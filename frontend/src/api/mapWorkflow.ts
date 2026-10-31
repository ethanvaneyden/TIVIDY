import type {
    Assignment,
    EducationInstanceData,
    HospitalInstanceData,
    Submission,
    Treatment,
    TreatmentContainer,
    WorkflowItemState,
    WorkflowPhase,
} from '../types/types';
import type { ApiContainer, ApiItem, ApiRoot } from './workflow';

const PHASES: WorkflowPhase[] = ['Intake', 'Active', 'Review', 'Publish'];

export function normalizeState(name: string): WorkflowItemState {
    switch (name.toLowerCase()) {
        case 'assigned': return 'assigned';
        case 'inprogress': return 'inprogress';
        case 'completed': return 'completed';
        case 'rejected': return 'rejected';
        default: return 'available';
    }
}

export function derivePhase(items: ApiItem[]): WorkflowPhase {
    if (items.length === 0) return 'Intake';
    const states = items.map((item) => normalizeState(item.state));
    if (states.every((state) => state === 'available')) return 'Intake';
    if (states.some((state) => state === 'available' || state === 'assigned' || state === 'inprogress')) {
        return 'Active';
    }
    const published = items.every((item, index) => states[index] === 'completed' && Boolean(item.reviewer));
    return published ? 'Publish' : 'Review';
}

export function phaseOf(container: ApiContainer): WorkflowPhase {
    const reported = PHASES.find((phase) => phase.toLowerCase() === container.phase?.toLowerCase());
    return reported ?? derivePhase(container.children);
}

export function mapTreatment(item: ApiItem): Treatment {
    return {
        id: item.id,
        name: item.name,
        age: item.age,
        diagnosis: item.diagnosis ?? '',
        state: normalizeState(item.state),
        assignee: item.assignee ?? '',
        reviewer: item.reviewer ?? '',
        severity: item.result,
        bed: item.bed,
        discharged: item.discharged,
    };
}

export function mapSubmission(item: ApiItem): Submission {
    return {
        id: item.id,
        name: item.name,
        state: normalizeState(item.state),
        assignee: item.assignee ?? '',
        reviewer: item.reviewer ?? '',
        mark: item.result,
    };
}

export function mapWard(container: ApiContainer): TreatmentContainer {
    return {
        id: container.id,
        name: container.name,
        phase: phaseOf(container),
        items: container.children.map(mapTreatment),
    };
}

export function mapAssignment(container: ApiContainer): Assignment {
    return {
        id: container.id,
        name: container.name,
        phase: phaseOf(container),
        items: container.children.map(mapSubmission),
    };
}

export function mapHospital(root: ApiRoot): HospitalInstanceData {
    return { name: root.name, wards: root.children.map(mapWard) };
}

export function mapEducation(root: ApiRoot): EducationInstanceData {
    return { name: root.name, assignments: root.children.map(mapAssignment) };
}
