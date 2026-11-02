import { describe, expect, it } from 'vitest';
import { installFakeApi } from '../test/fakeApi';
import {
    addContainer,
    addItem,
    approveContainer,
    completeItem,
    deleteInstance,
    getHistory,
    getState,
    instanceName,
    listInstances,
    openContainer,
    overrideItem,
    startInstance,
    startItem,
} from './workflow';

function lastCall(calls: ReturnType<typeof installFakeApi>) {
    return calls[calls.length - 1];
}

describe('instanceName', () => {
    it('strips the id prefix the server adds', () => {
        expect(instanceName('hc_0_City Hospital')).toBe('City Hospital');
        expect(instanceName('edu_12_COS 214')).toBe('COS 214');
        expect(instanceName('edu_3_a_b')).toBe('a_b');
    });

    it('keeps ids that do not follow the server format', () => {
        expect(instanceName('custom-id')).toBe('custom-id');
        expect(instanceName('hc_1_')).toBe('hc_1_');
    });
});

describe('routes match RestServer', () => {
    it('lists, starts and deletes instances per scenario', async () => {
        const calls = installFakeApi((call) => {
            if (call.path.endsWith('/start')) return { json: { instanceId: 'edu_0_X' } };
            if (call.path.endsWith('/instances')) return { json: ['edu_0_X'] };
            return { json: { result: 'Success' } };
        });

        expect(await listInstances('education')).toEqual(['edu_0_X']);
        expect(lastCall(calls)).toMatchObject({ method: 'GET', path: '/api/education/instances' });

        expect(await startInstance('education', 'X')).toBe('edu_0_X');
        expect(lastCall(calls)).toMatchObject({ method: 'POST', path: '/api/education/start', body: { name: 'X' } });

        await deleteInstance('healthcare', 'hc_0_Y');
        expect(lastCall(calls)).toMatchObject({ method: 'DELETE', path: '/api/healthcare/hc_0_Y' });
    });

    it('reads state and history', async () => {
        const calls = installFakeApi(() => ({ json: [] }));
        await getState('healthcare', 'hc_0_Y');
        expect(lastCall(calls)).toMatchObject({ method: 'GET', path: '/api/healthcare/hc_0_Y/state' });
        await getHistory('education', 'edu_0_X');
        expect(lastCall(calls)).toMatchObject({ method: 'GET', path: '/api/education/edu_0_X/history' });
    });

    it('adds containers through submit for education and admit for healthcare', async () => {
        const calls = installFakeApi(() => ({ json: { result: 'Success' } }));
        await addContainer('education', 'edu_0_X', 'Test 1');
        expect(lastCall(calls)).toMatchObject({
            path: '/api/education/edu_0_X/submit',
            body: { containerName: 'Test 1' },
        });
        await addContainer('healthcare', 'hc_0_Y', 'ICU');
        expect(lastCall(calls)).toMatchObject({
            path: '/api/healthcare/hc_0_Y/admit',
            body: { containerName: 'ICU' },
        });
    });

    it('drives items and containers through the shared action routes', async () => {
        const calls = installFakeApi(() => ({ json: { result: 'Success' } }));

        await startItem('healthcare', 'hc_0_Y', 'ITEM-1');
        expect(lastCall(calls)).toMatchObject({ method: 'POST', path: '/api/healthcare/hc_0_Y/items/ITEM-1/start' });

        await completeItem('education', 'edu_0_X', 'SUB-A', 78);
        expect(lastCall(calls)).toMatchObject({
            path: '/api/education/edu_0_X/items/SUB-A/complete',
            body: { value: 78 },
        });

        await approveContainer('healthcare', 'hc_0_Y', 'W1');
        expect(lastCall(calls)).toMatchObject({ path: '/api/healthcare/hc_0_Y/containers/W1/approve' });

        await overrideItem('education', 'edu_0_X', 'SUB-G', 55, false);
        expect(lastCall(calls)).toMatchObject({
            path: '/api/education/edu_0_X/items/SUB-G/override',
            body: { value: 55, accept: 0 },
        });

        await overrideItem('healthcare', 'hc_0_Y', 'ITEM-2', 0, true);
        expect(lastCall(calls)).toMatchObject({ body: { value: 0, accept: 1 } });
    });

    it('encodes ids that contain spaces', async () => {
        const calls = installFakeApi(() => ({ json: [] }));
        await getState('education', 'edu_0_COS 214');
        expect(lastCall(calls).path).toBe('/api/education/edu_0_COS 214/state');
    });
});

describe('proposed routes', () => {
    it('opens containers and adds items with the documented payloads', async () => {
        const calls = installFakeApi(() => ({ json: { result: 'Success' } }));

        await openContainer('healthcare', 'hc_0_Y', 'W1');
        expect(lastCall(calls)).toMatchObject({ path: '/api/healthcare/hc_0_Y/containers/W1/open' });

        await addItem('healthcare', 'hc_0_Y', 'W1', { name: 'Anele', age: 40, diagnosis: 'Flu' });
        expect(lastCall(calls)).toMatchObject({
            path: '/api/healthcare/hc_0_Y/containers/W1/patients',
            body: { name: 'Anele', age: 40, diagnosis: 'Flu' },
        });

        await addItem('education', 'edu_0_X', 'A1', { studentName: 'Student A' });
        expect(lastCall(calls)).toMatchObject({
            path: '/api/education/edu_0_X/containers/A1/submissions',
            body: { studentName: 'Student A' },
        });
    });
});
