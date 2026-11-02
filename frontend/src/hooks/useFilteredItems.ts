import { useEffect, useState } from 'react';
import { messageOf } from '../api/client';
import { getFilteredItems, type FilteredItem } from '../api/workflow';
import type { ScenarioId } from '../types/types';

export interface FilteredItemsState {
    items: FilteredItem[];
    loading: boolean;
    error?: string;
}

interface Loaded {
    key: string;
    items: FilteredItem[];
    error?: string;
}

/**
 * Items in a given lifecycle state, fetched from the backend's
 * StateFilterIterator. Loading is derived from the request key, so the
 * effect only ever sets state from the asynchronous result.
 */
export function useFilteredItems(
    scenario: ScenarioId,
    instanceId: string,
    state: string,
): FilteredItemsState {
    const [loaded, setLoaded] = useState<Loaded>();
    const key = `${scenario}|${instanceId}|${state}`;

    useEffect(() => {
        if (!state) return;
        let cancelled = false;
        getFilteredItems(scenario, instanceId, state)
            .then((items) => {
                if (!cancelled) setLoaded({ key, items });
            })
            .catch((error: unknown) => {
                if (!cancelled) setLoaded({ key, items: [], error: messageOf(error) });
            });
        return () => { cancelled = true; };
    }, [scenario, instanceId, state, key]);

    if (!state) return { items: [], loading: false };
    if (loaded?.key !== key) return { items: [], loading: true };
    return { items: loaded.items, loading: false, error: loaded.error };
}
