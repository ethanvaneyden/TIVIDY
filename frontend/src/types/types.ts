export type ScenarioId = 'education' | 'healthcare';
export type WorkflowItemState = 'available' | 'assigned' | 'inprogress' | 'completed' | 'rejected';
export type WorkflowPhase = 'Intake' | 'Active' | 'Review' | 'Publish';

export interface ProgressContainer {
    id: string;
    name: string;
    phase: string;
    total: number;
    completed: number;
    reviewed: number;
    rejected: number;
    percentComplete: number;
}

export interface ProgressReport {
    totalItems: number;
    completedItems: number;
    percentComplete: number;
    states: Record<string, number>;
    containers: ProgressContainer[];
}

export interface ScenarioInstance {
    id: string;
    name: string;
}

export interface EducationInstanceData {
    name: string;
    assignments: Assignment[];
}

export interface HospitalInstanceData {
    name: string;
    wards: TreatmentContainer[];
}

export interface Assignment {
    id: string;
    name: string;
    phase: WorkflowPhase;
    items: Submission[];
}

export interface TreatmentContainer {
    id: string;
    name: string;
    phase: WorkflowPhase;
    items: Treatment[];
}

export interface Submission {
    id: string;
    name: string;
    state: WorkflowItemState;
    assignee: string;
    reviewer: string;
    mark?: number;
    rejectionReason?: string;
}

export interface Treatment {
    id: string;
    name: string;
    age?: number;
    diagnosis: string;
    state: WorkflowItemState;
    assignee: string;
    reviewer: string;
    severity?: number;
    rejectionReason?: string;
    bed?: string;
    discharged?: boolean;
}
