import { vi } from 'vitest';

export interface RecordedCall {
    method: string;
    path: string;
    body: unknown;
}

export interface FakeReply {
    status?: number;
    json: unknown;
}

export type FakeHandler = (call: RecordedCall) => FakeReply | undefined;

export function installFakeApi(handler: FakeHandler): RecordedCall[] {
    const calls: RecordedCall[] = [];
    vi.stubGlobal('fetch', vi.fn(async (input: RequestInfo | URL, init?: RequestInit) => {
        const url = new URL(String(input));
        const call: RecordedCall = {
            method: init?.method ?? 'GET',
            path: decodeURIComponent(url.pathname),
            body: typeof init?.body === 'string' ? JSON.parse(init.body) : undefined,
        };
        calls.push(call);
        const reply = handler(call) ?? { status: 404, json: { result: 'NotFound' } };
        return new Response(JSON.stringify(reply.json), { status: reply.status ?? 200 });
    }));
    return calls;
}

export function item(id: string, name: string, state: string, extra: Record<string, unknown> = {}) {
    return { id, type: 'Item', name, state, reviewMethod: '', assignee: null, reviewer: null, ...extra };
}

export function root(type: string, name: string, containers: unknown[]) {
    return { id: 'root', type, name, children: containers };
}

export function container(type: string, id: string, name: string, phase: string, items: unknown[]) {
    return { id, type, name, phase, children: items };
}
