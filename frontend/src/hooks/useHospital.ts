import {
    addContainer,
    addItem,
    approveContainer,
    completeItem,
    openContainer,
    overrideItem,
    startItem,
    type NewPatient,
} from '../api/workflow';
import { mapHospital } from '../api/mapWorkflow';
import type { HospitalInstanceData } from '../types/types';
import { useWorkflowResource } from './useWorkflowResource';

export interface HospitalController {
    hospital?: HospitalInstanceData;
    history: string[];
    loading: boolean;
    busy: boolean;
    loadError?: string;
    actionError?: string;
    clearActionError: () => void;
    refresh: () => Promise<void>;
    addWard: (name: string) => Promise<boolean>;
    addPatient: (wardId: string, patient: NewPatient) => Promise<boolean>;
    openWard: (wardId: string) => Promise<boolean>;
    startTreatment: (itemId: string) => Promise<boolean>;
    completeTreatment: (itemId: string, severity: number) => Promise<boolean>;
    approveWard: (wardId: string) => Promise<boolean>;
    overrideTreatment: (itemId: string, severity: number, accept: boolean) => Promise<boolean>;
}

export function useHospital(instanceId: string): HospitalController {
    const resource = useWorkflowResource('healthcare', instanceId, mapHospital);
    const { run } = resource;

    return {
        hospital: resource.data,
        history: resource.history,
        loading: resource.loading,
        busy: resource.busy,
        loadError: resource.loadError,
        actionError: resource.actionError,
        clearActionError: resource.clearActionError,
        refresh: resource.refresh,
        addWard: (name) => run(() => addContainer('healthcare', instanceId, name)),
        addPatient: (wardId, patient) => run(() => addItem('healthcare', instanceId, wardId, patient)),
        openWard: (wardId) => run(() => openContainer('healthcare', instanceId, wardId)),
        startTreatment: (itemId) => run(() => startItem('healthcare', instanceId, itemId)),
        completeTreatment: (itemId, severity) => run(() => completeItem('healthcare', instanceId, itemId, severity)),
        approveWard: (wardId) => run(() => approveContainer('healthcare', instanceId, wardId)),
        overrideTreatment: (itemId, severity, accept) =>
            run(() => overrideItem('healthcare', instanceId, itemId, severity, accept)),
    };
}
