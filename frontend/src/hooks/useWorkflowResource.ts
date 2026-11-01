import { useCallback, useEffect, useState } from 'react';
import { messageOf } from '../api/client';
import { getHistory, getState, type ApiRoot } from '../api/workflow';
import type { ScenarioId } from '../types/types';

export interface WorkflowResource<T> {
    data?: T;
    history: string[];
    loading: boolean;
    busy: boolean;
    loadError?: string;
    actionError?: string;
    clearActionError: () => void;
    refresh: () => Promise<void>;
    run: (action: () => Promise<unknown>) => Promise<boolean>;
}

export function useWorkflowResource<T>(
    scenario: ScenarioId,
    instanceId: string,
    map: (root: ApiRoot) => T,
): WorkflowResource<T> {
    const [data, setData] = useState<T>();
    const [history, setHistory] = useState<string[]>([]);
    const [loading, setLoading] = useState(true);
    const [busy, setBusy] = useState(false);
    const [loadError, setLoadError] = useState<string>();
    const [actionError, setActionError] = useState<string>();

    const fetchSnapshot = useCallback(async () => {
        const [root, entries] = await Promise.all([
            getState(scenario, instanceId),
            getHistory(scenario, instanceId),
        ]);
        return { data: map(root), history: entries };
    }, [scenario, instanceId, map]);

    const load = useCallback(async () => {
        const snapshot = await fetchSnapshot();
        setData(snapshot.data);
        setHistory(snapshot.history);
        setLoadError(undefined);
    }, [fetchSnapshot]);

    useEffect(() => {
        let cancelled = false;
        fetchSnapshot()
            .then((snapshot) => {
                if (cancelled) return;
                setData(snapshot.data);
                setHistory(snapshot.history);
                setLoadError(undefined);
            })
            .catch((error: unknown) => {
                if (!cancelled) setLoadError(messageOf(error));
            })
            .finally(() => {
                if (!cancelled) setLoading(false);
            });
        return () => {
            cancelled = true;
        };
    }, [fetchSnapshot]);

    const refresh = useCallback(async () => {
        setLoading(true);
        try {
            await load();
        } catch (error) {
            setLoadError(messageOf(error));
        } finally {
            setLoading(false);
        }
    }, [load]);

    const run = useCallback(async (action: () => Promise<unknown>): Promise<boolean> => {
        setBusy(true);
        setActionError(undefined);
        let ok = true;
        try {
            await action();
        } catch (error) {
            ok = false;
            setActionError(messageOf(error));
        }
        try {
            await load();
        } catch (error) {
            setActionError((previous) => previous ?? messageOf(error));
        }
        setBusy(false);
        return ok;
    }, [load]);

    const clearActionError = useCallback(() => setActionError(undefined), []);

    return { data, history, loading, busy, loadError, actionError, clearActionError, refresh, run };
}
