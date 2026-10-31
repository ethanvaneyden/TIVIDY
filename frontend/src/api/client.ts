export const API_BASE: string = import.meta.env.VITE_API_URL ?? 'http://localhost:8080';

const RESULT_MESSAGES: Record<string, string> = {
    InvalidTransition: 'That action is not allowed in the current state.',
    Rejected: 'The action was rejected.',
    IntegrationFailure: 'An external system failed. Try again shortly.',
    NotFound: 'The requested item or instance was not found.',
};

export class ApiError extends Error {
    status: number;
    code: string;

    constructor(status: number, code: string, message: string) {
        super(message);
        this.name = 'ApiError';
        this.status = status;
        this.code = code;
    }
}

function readString(body: unknown, key: string): string | undefined {
    if (typeof body === 'object' && body !== null && key in body) {
        const value = (body as Record<string, unknown>)[key];
        return typeof value === 'string' ? value : undefined;
    }
    return undefined;
}

async function request<T>(path: string, init?: RequestInit): Promise<T> {
    let response: Response;
    try {
        response = await fetch(`${API_BASE}${path}`, {
            headers: { 'Content-Type': 'application/json' },
            ...init,
        });
    } catch {
        throw new ApiError(0, 'NetworkError', 'Cannot reach the TIVIDY server. Is it running?');
    }

    const text = await response.text();
    let body: unknown;
    if (text) {
        try {
            body = JSON.parse(text);
        } catch {
            body = undefined;
        }
    }

    if (!response.ok) {
        const code = readString(body, 'result') ?? readString(body, 'error') ?? `HTTP${response.status}`;
        const detail = readString(body, 'message');
        throw new ApiError(
            response.status,
            code,
            RESULT_MESSAGES[code] ?? detail ?? `Request failed (${response.status}).`,
        );
    }

    return body as T;
}

export function get<T>(path: string): Promise<T> {
    return request<T>(path);
}

export function post<T>(path: string, body?: unknown): Promise<T> {
    return request<T>(path, { method: 'POST', body: JSON.stringify(body ?? {}) });
}

export function del<T>(path: string): Promise<T> {
    return request<T>(path, { method: 'DELETE' });
}

export function messageOf(error: unknown): string {
    return error instanceof Error ? error.message : 'Something went wrong.';
}
