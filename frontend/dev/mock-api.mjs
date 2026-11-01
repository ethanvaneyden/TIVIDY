import { createServer } from 'node:http';

const PORT = Number(process.env.PORT ?? 8080);

const SCENARIOS = {
    healthcare: {
        idPrefix: 'hc',
        rootType: 'hospital',
        containerType: 'ward',
        defaultContainer: 'General Ward',
        itemCollection: 'patients',
        workers: ['Doctor-1', 'Doctor-2', 'Doctor-3'],
        settler: 'ChiefOfMedicine-1',
        approverFor(value) {
            if (value >= 1 && value <= 3) return 'TriageNurse-1';
            if (value >= 4 && value <= 6) return 'AdmittingDoctor-1';
            if (value >= 7 && value <= 10) return 'ChiefOfMedicine-1';
            return undefined;
        },
        canSettleWith(value) {
            return Number.isInteger(value) && value >= 1 && value <= 10;
        },
        seed: [
            { name: 'Karabo Sithole', age: 34, diagnosis: 'Sprained ankle' },
            { name: 'Anele Zulu', age: 58, diagnosis: 'Pneumonia' },
            { name: 'Thabo Molefe', age: 71, diagnosis: 'Cardiac arrhythmia' },
        ],
        itemFields(body) {
            return { name: body.name, age: body.age, diagnosis: body.diagnosis };
        },
    },
    education: {
        idPrefix: 'edu',
        rootType: 'course',
        containerType: 'assignment',
        defaultContainer: 'Assignment 1',
        itemCollection: 'submissions',
        workers: ['Alice', 'Bob', 'Charlie'],
        settler: 'Dr Dlamini',
        approverFor(value) {
            if (value < 40 || value > 100) return undefined;
            if (value <= 49) return 'Dr Mokoena';
            if (value <= 70) return 'Alice';
            return 'Dr Dlamini';
        },
        canSettleWith(value) {
            return Number.isInteger(value) && value >= 0 && value <= 100;
        },
        seed: [{ name: 'Student A' }, { name: 'Student B' }, { name: 'Student C' }],
        itemFields(body) {
            return { name: body.studentName };
        },
    },
};

const stores = { healthcare: new Map(), education: new Map() };
const counters = { healthcare: 0, education: 0 };

function addContainer(instance, name) {
    instance.containerCounter += 1;
    const container = { id: `C${instance.containerCounter}`, name, phase: 'Intake', items: [] };
    instance.containers.push(container);
    instance.history.push(`${container.id} created`);
    return container;
}

function addItem(instance, container, fields) {
    instance.itemCounter += 1;
    const item = {
        id: `ITEM-${instance.itemCounter}`,
        ...fields,
        state: 'Available',
        assignee: null,
        reviewer: null,
        result: undefined,
    };
    container.items.push(item);
    instance.history.push(`${item.id} added`);
    return item;
}

function newInstance(config, name) {
    const instance = { name, containers: [], history: [], containerCounter: 0, itemCounter: 0, turn: 0 };
    const container = addContainer(instance, config.defaultContainer);
    for (const fields of config.seed) addItem(instance, container, fields);
    return instance;
}

function findItem(instance, itemId) {
    for (const container of instance.containers) {
        const item = container.items.find((candidate) => candidate.id === itemId);
        if (item) return { container, item };
    }
    return undefined;
}

function publishIfReady(instance, container) {
    const ready = container.items.every((item) => item.state === 'Completed' && item.reviewer);
    if (container.phase === 'Review' && ready) {
        container.phase = 'Publish';
        instance.history.push(`${container.id} moved to Publish`);
    }
}

function stateOf(instance) {
    return {
        id: 'root',
        type: instance.rootType,
        name: instance.name,
        children: instance.containers.map((container) => ({
            id: container.id,
            type: instance.containerType,
            name: container.name,
            phase: container.phase,
            children: container.items.map((item) => ({
                id: item.id,
                type: 'Item',
                name: item.name,
                state: item.state,
                reviewMethod: item.reviewer ? 'Batch' : '',
                assignee: item.assignee,
                reviewer: item.reviewer,
                result: item.result,
                diagnosis: item.diagnosis,
                age: item.age,
            })),
        })),
    };
}

function send(res, status, body) {
    res.writeHead(status, {
        'Content-Type': 'application/json',
        'Access-Control-Allow-Origin': '*',
        'Access-Control-Allow-Methods': 'GET, POST, DELETE, OPTIONS',
        'Access-Control-Allow-Headers': 'Content-Type',
    });
    res.end(JSON.stringify(body));
}

const ok = (res) => send(res, 200, { result: 'Success' });
const invalid = (res) => send(res, 409, { result: 'InvalidTransition' });
const notFound = (res) => send(res, 404, { result: 'NotFound' });

function readBody(req) {
    return new Promise((resolve) => {
        let data = '';
        req.on('data', (chunk) => { data += chunk; });
        req.on('end', () => {
            try { resolve(data ? JSON.parse(data) : {}); } catch { resolve({}); }
        });
    });
}

function handleContainer(config, instance, container, action, body, res) {
    if (action === 'open') {
        if (container.phase !== 'Intake' || container.items.length === 0) return invalid(res);
        container.phase = 'Active';
        instance.history.push(`${container.id} moved to Active`);
        for (const item of container.items) {
            item.assignee = config.workers[instance.turn % config.workers.length];
            instance.turn += 1;
            item.state = 'Assigned';
            instance.history.push(`${item.id} assigned to ${item.assignee}`);
        }
        return ok(res);
    }
    if (action === config.itemCollection) {
        if (container.phase !== 'Intake') return invalid(res);
        addItem(instance, container, config.itemFields(body));
        return ok(res);
    }
    if (action === 'approve') {
        if (container.phase !== 'Review') return invalid(res);
        for (const item of container.items) {
            if (item.state !== 'Completed' || item.reviewer) continue;
            const approver = config.approverFor(item.result);
            if (approver) {
                item.reviewer = approver;
                instance.history.push(`${item.id} approved`);
            } else {
                item.state = 'Rejected';
                instance.history.push(`${item.id} rejected: ${item.result} cannot be auto-approved`);
            }
        }
        publishIfReady(instance, container);
        return ok(res);
    }
    return notFound(res);
}

function handleItem(config, instance, found, action, body, res) {
    const { container, item } = found;
    if (action === 'start') {
        if (container.phase !== 'Active' || item.state !== 'Assigned') return invalid(res);
        item.state = 'InProgress';
        instance.history.push(`${item.id} started`);
        return ok(res);
    }
    if (action === 'complete') {
        if (container.phase !== 'Active' || item.state !== 'InProgress') return invalid(res);
        item.state = 'Completed';
        item.result = Number.isInteger(body.value) ? body.value : 0;
        instance.history.push(`${item.id} completed with ${item.result}`);
        if (container.items.every((candidate) => candidate.state === 'Completed')) {
            container.phase = 'Review';
            instance.history.push(`${container.id} moved to Review`);
        }
        return ok(res);
    }
    if (action === 'override') {
        if (container.phase !== 'Review' || item.state !== 'Rejected') return invalid(res);
        const finalValue = body.accept ? item.result : body.value;
        if (!config.canSettleWith(finalValue)) return send(res, 409, { result: 'Rejected' });
        item.result = finalValue;
        item.state = 'Completed';
        item.reviewer = config.settler;
        instance.history.push(`${item.id} settled by ${config.settler} with ${finalValue}`);
        publishIfReady(instance, container);
        return ok(res);
    }
    return notFound(res);
}

const server = createServer(async (req, res) => {
    if (req.method === 'OPTIONS') return send(res, 200, {});
    const url = new URL(req.url, `http://localhost:${PORT}`);
    const parts = url.pathname.split('/').filter(Boolean).map(decodeURIComponent);
    if (parts[0] !== 'api') return notFound(res);

    const config = SCENARIOS[parts[1]];
    if (!config) return notFound(res);
    const store = stores[parts[1]];
    const body = req.method === 'POST' ? await readBody(req) : {};

    if (parts[2] === 'instances' && req.method === 'GET') return send(res, 200, [...store.keys()]);
    if (parts[2] === 'start' && req.method === 'POST') {
        const name = typeof body.name === 'string' ? body.name : 'untitled';
        const id = `${config.idPrefix}_${counters[parts[1]]}_${name}`;
        counters[parts[1]] += 1;
        const instance = newInstance(config, name);
        instance.rootType = config.rootType;
        instance.containerType = config.containerType;
        store.set(id, instance);
        return send(res, 200, { instanceId: id });
    }

    const instance = store.get(parts[2]);
    if (!instance) return notFound(res);
    const rest = parts.slice(3);

    if (rest.length === 0 && req.method === 'DELETE') {
        store.delete(parts[2]);
        return ok(res);
    }
    if (rest[0] === 'state' && req.method === 'GET') return send(res, 200, stateOf(instance));
    if (rest[0] === 'history' && req.method === 'GET') return send(res, 200, instance.history);
    if (req.method !== 'POST') return notFound(res);

    const adder = parts[1] === 'education' ? 'submit' : 'admit';
    if (rest[0] === adder) {
        addContainer(instance, typeof body.containerName === 'string' ? body.containerName : 'Untitled');
        return ok(res);
    }
    if (rest[0] === 'containers') {
        const container = instance.containers.find((candidate) => candidate.id === rest[1]);
        return container ? handleContainer(config, instance, container, rest[2], body, res) : notFound(res);
    }
    if (rest[0] === 'items') {
        const found = findItem(instance, rest[1]);
        return found ? handleItem(config, instance, found, rest[2], body, res) : notFound(res);
    }
    return notFound(res);
});

server.listen(PORT, () => {
    console.log(`TIVIDY mock API on http://localhost:${PORT}`);
});
