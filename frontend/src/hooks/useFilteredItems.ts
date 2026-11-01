import { useEffect, useState } from 'react';
import { messageOf } from '../api/client';
import { getFilteredItems, type FilteredItem } from '../api/workflow';
import type { ScenarioId } from '../types/types';

export interface FilteredItemsState {
    items: FilteredItem[];
    loading: boolean;
    error?: string;
}

export function useFilteredItems(
    scenario: ScenarioId,
    instanceId: string,
    state: string,
): FilteredItemsState {
    const [items, setItems] = useState<FilteredItem[]>([]);
    const [loading, setLoading] = useState(false);
    const [error, setError] = useState<string>();

    useEffect(() => {
        if (!state) {
            setItems([]);
            setError(undefined);
            return;
        }
        let cancelled = false;
        setLoading(true);
        getFilteredItems(scenario, instanceId, state)
            .then((res) => {
                if (cancelled) return;
                setItems(res);
                setError(undefined);
            })
            .catch((err: unknown) => {
                if (!cancelled) setError(messageOf(err));
            })
            .finally(() => {
                if (!cancelled) setLoading(false);
            });
        return () => { cancelled = true; };
    }, [scenario, instanceId, state]);

    return { items, loading, error };
}