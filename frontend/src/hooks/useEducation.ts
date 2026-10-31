import {
    addContainer,
    addItem,
    approveContainer,
    completeItem,
    openContainer,
    overrideItem,
    startItem,
    type NewSubmission,
} from '../api/workflow';
import { mapEducation } from '../api/mapWorkflow';
import type { EducationInstanceData } from '../types/types';
import { useWorkflowResource } from './useWorkflowResource';

export interface EducationController {
    education?: EducationInstanceData;
    history: string[];
    loading: boolean;
    busy: boolean;
    loadError?: string;
    actionError?: string;
    clearActionError: () => void;
    refresh: () => Promise<void>;
    addAssignment: (name: string) => Promise<boolean>;
    addSubmission: (assignmentId: string, submission: NewSubmission) => Promise<boolean>;
    openAssignment: (assignmentId: string) => Promise<boolean>;
    startSubmission: (itemId: string) => Promise<boolean>;
    completeSubmission: (itemId: string, mark: number) => Promise<boolean>;
    approveAssignment: (assignmentId: string) => Promise<boolean>;
    overrideSubmission: (itemId: string, mark: number, accept: boolean) => Promise<boolean>;
}

export function useEducation(instanceId: string): EducationController {
    const resource = useWorkflowResource('education', instanceId, mapEducation);
    const { run } = resource;

    return {
        education: resource.data,
        history: resource.history,
        loading: resource.loading,
        busy: resource.busy,
        loadError: resource.loadError,
        actionError: resource.actionError,
        clearActionError: resource.clearActionError,
        refresh: resource.refresh,
        addAssignment: (name) => run(() => addContainer('education', instanceId, name)),
        addSubmission: (assignmentId, submission) =>
            run(() => addItem('education', instanceId, assignmentId, submission)),
        openAssignment: (assignmentId) => run(() => openContainer('education', instanceId, assignmentId)),
        startSubmission: (itemId) => run(() => startItem('education', instanceId, itemId)),
        completeSubmission: (itemId, mark) => run(() => completeItem('education', instanceId, itemId, mark)),
        approveAssignment: (assignmentId) => run(() => approveContainer('education', instanceId, assignmentId)),
        overrideSubmission: (itemId, mark, accept) =>
            run(() => overrideItem('education', instanceId, itemId, mark, accept)),
    };
}
