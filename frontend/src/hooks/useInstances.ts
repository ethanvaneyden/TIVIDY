import { useCallback, useEffect, useState } from 'react';
import { messageOf } from '../api/client';
import { deleteInstance, instanceName, listInstances, startInstance } from '../api/workflow';
import type { ScenarioId, ScenarioInstance } from '../types/types';

export interface InstancesController {
    instances: ScenarioInstance[];
    loading: boolean;
    busy: boolean;
    loadError?: string;
    actionError?: string;
    clearActionError: () => void;
    refresh: () => Promise<void>;
    create: (name: string) => Promise<string | undefined>;
    remove: (instanceId: string) => Promise<boolean>;
}

function toInstances(ids: string[]): ScenarioInstance[] {
    return ids.map((id) => ({ id, name: instanceName(id) }));
}

export function useInstances(scenario: ScenarioId): InstancesController {
    const [instances, setInstances] = useState<ScenarioInstance[]>([]);
    const [loading, setLoading] = useState(true);
    const [busy, setBusy] = useState(false);
    const [loadError, setLoadError] = useState<string>();
    const [actionError, setActionError] = useState<string>();

    useEffect(() => {
        let cancelled = false;
        listInstances(scenario)
            .then((ids) => {
                if (cancelled) return;
                setInstances(toInstances(ids));
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
    }, [scenario]);

    const refresh = useCallback(async () => {
        setLoading(true);
        try {
            setInstances(toInstances(await listInstances(scenario)));
            setLoadError(undefined);
        } catch (error) {
            setLoadError(messageOf(error));
        } finally {
            setLoading(false);
        }
    }, [scenario]);

    const reload = useCallback(async () => {
        try {
            setInstances(toInstances(await listInstances(scenario)));
        } catch (error) {
            setActionError((previous) => previous ?? messageOf(error));
        }
    }, [scenario]);

    const create = useCallback(async (name: string): Promise<string | undefined> => {
        setBusy(true);
        setActionError(undefined);
        let id: string | undefined;
        try {
            id = await startInstance(scenario, name);
        } catch (error) {
            setActionError(messageOf(error));
        }
        await reload();
        setBusy(false);
        return id;
    }, [scenario, reload]);

    const remove = useCallback(async (instanceId: string): Promise<boolean> => {
        setBusy(true);
        setActionError(undefined);
        let ok = true;
        try {
            await deleteInstance(scenario, instanceId);
        } catch (error) {
            ok = false;
            setActionError(messageOf(error));
        }
        await reload();
        setBusy(false);
        return ok;
    }, [scenario, reload]);

    const clearActionError = useCallback(() => setActionError(undefined), []);

    return { instances, loading, busy, loadError, actionError, clearActionError, refresh, create, remove };
}
