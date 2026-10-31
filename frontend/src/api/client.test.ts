import { afterEach, describe, expect, it, vi } from 'vitest';
import { ApiError, get, messageOf, post } from './client';

function respond(status: number, body: string): Response {
    return new Response(body, { status });
}

afterEach(() => {
    vi.unstubAllGlobals();
});

describe('request handling', () => {
    it('returns parsed JSON on success', async () => {
        vi.stubGlobal('fetch', vi.fn().mockResolvedValue(respond(200, '["HC1","HC2"]')));
        await expect(get<string[]>('/api/healthcare/instances')).resolves.toEqual(['HC1', 'HC2']);
    });

    it('sends POST bodies as JSON', async () => {
        const fetchMock = vi.fn().mockResolvedValue(respond(200, '{"result":"Success"}'));
        vi.stubGlobal('fetch', fetchMock);
        await post('/api/healthcare/x/admit', { containerName: 'ICU' });
        const init = fetchMock.mock.calls[0][1] as RequestInit;
        expect(init.method).toBe('POST');
        expect(init.body).toBe('{"containerName":"ICU"}');
    });

    it('turns a 409 InvalidTransition into a readable ApiError', async () => {
        vi.stubGlobal('fetch', vi.fn().mockResolvedValue(respond(409, '{"result":"InvalidTransition"}')));
        const error = await post('/x').catch((e: unknown) => e);
        expect(error).toBeInstanceOf(ApiError);
        expect((error as ApiError).status).toBe(409);
        expect((error as ApiError).code).toBe('InvalidTransition');
        expect((error as ApiError).message).toMatch(/not allowed/);
    });

    it('reads error and message fields from serializeError bodies', async () => {
        vi.stubGlobal('fetch', vi.fn().mockResolvedValue(respond(404, '{"error":"Odd","message":"custom text"}')));
        const error = await get('/x').catch((e: unknown) => e);
        expect((error as ApiError).code).toBe('Odd');
        expect((error as ApiError).message).toBe('custom text');
    });

    it('survives an empty or non-JSON error body', async () => {
        vi.stubGlobal('fetch', vi.fn().mockResolvedValue(respond(500, 'boom')));
        const error = await get('/x').catch((e: unknown) => e);
        expect((error as ApiError).code).toBe('HTTP500');
    });

    it('reports an unreachable server as a NetworkError', async () => {
        vi.stubGlobal('fetch', vi.fn().mockRejectedValue(new TypeError('failed')));
        const error = await get('/x').catch((e: unknown) => e);
        expect((error as ApiError).code).toBe('NetworkError');
        expect((error as ApiError).status).toBe(0);
    });
});

describe('messageOf', () => {
    it('uses an Error message and falls back for anything else', () => {
        expect(messageOf(new Error('nope'))).toBe('nope');
        expect(messageOf('weird')).toBe('Something went wrong.');
    });
});
