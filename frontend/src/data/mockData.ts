import type {
    EducationInstanceData,
    HospitalInstanceData,
    ScenarioId,
    ScenarioInstance,
    Submission,
    Treatment,
} from '../types/types';

const instances: Record<ScenarioId, ScenarioInstance[]> = {
    education: [
        { id: 'INS001', name: 'COS214 · 2026' },
        { id: 'INS002', name: 'COS214 · 2025' },
    ],
    healthcare: [
        { id: 'HC001', name: 'Tividy General Hospital' },
        { id: 'HC002', name: 'City Medical Centre' },
    ],
};

const firstTestItems: Submission[] = [
    { id: 'SUB-001', name: 'Liam van der Merwe', state: 'completed', assignee: 'Mr Smith', reviewer: '', mark: 68 },
    { id: 'SUB-002', name: 'Ayesha Khan', state: 'completed', assignee: 'Mr Jackson', reviewer: '', mark: 82 },
    { id: 'SUB-003', name: 'Thabo Mokoena', state: 'rejected', assignee: 'Mr Jackson', reviewer: '', mark: 32 },
    { id: 'SUB-004', name: 'Mei Chen', state: 'assigned', assignee: 'Mr Smith', reviewer: '' },
    { id: 'SUB-005', name: 'Olivia Smith', state: 'inprogress', assignee: 'Mr Jackson', reviewer: '' },
];

const secondTestItems: Submission[] = [
    { id: 'SUB-006', name: 'Liam van der Merwe', state: 'completed', assignee: 'Mr Smith', reviewer: '', mark: 74 },
    { id: 'SUB-007', name: 'Ayesha Khan', state: 'assigned', assignee: 'Mr Jackson', reviewer: '' },
];

const treatmentItems: Treatment[] = [
    { id: 'TRT-1042', name: 'Amara Nkosi', age: 46, diagnosis: 'Acute appendicitis', state: 'completed', assignee: 'Dr Patel', reviewer: '', severity: 7 },
    { id: 'TRT-1043', name: 'Daniel Jacobs', age: 71, diagnosis: 'Community-acquired pneumonia', state: 'completed', assignee: 'Dr Mokoena', reviewer: '', severity: 5 },
    { id: 'TRT-1044', name: 'Grace Williams', age: 32, diagnosis: 'Post-operative observation', state: 'rejected', assignee: 'Dr Patel', reviewer: '', severity: 0 },
    { id: 'TRT-1045', name: 'Sipho Dlamini', age: 58, diagnosis: 'Chest pain assessment', state: 'assigned', assignee: 'Dr Naidoo', reviewer: '' },
];

export function listMockInstances(scenario: ScenarioId): ScenarioInstance[] {
    return instances[scenario].map((instance) => ({ ...instance }));
}

export function getMockInstance(instanceId: string): ScenarioInstance | undefined {
    const instance = [...instances.education, ...instances.healthcare]
        .find((candidate) => candidate.id === instanceId);
    return instance ? { ...instance } : undefined;
}

export function getMockEducationInstance(instanceId: string): EducationInstanceData {
    const instance = instances.education.find(({ id }) => id === instanceId);
    return {
        name: instance?.name ?? 'New instance',
        assignments: instance ? [
            {
                id: `${instanceId}-TEST-1`,
                name: 'Test 1',
                phase: 'Active',
                items: firstTestItems.map((item) => ({ ...item })),
            },
            {
                id: `${instanceId}-TEST-2`,
                name: 'Test 2',
                phase: 'Active',
                items: secondTestItems.map((item) => ({ ...item })),
            },
        ] : [],
    };
}

export function getMockHospital(instanceId: string): HospitalInstanceData {
    const instance = instances.healthcare.find(({ id }) => id === instanceId);
    const isDemoInstance = instance?.id === 'HC001' || instance?.id === 'HC002';
    const makeItems = (prefix: string) => isDemoInstance
        ? treatmentItems.map((item, index) => ({
            ...item,
            id: `${prefix}-${String(index + 1).padStart(3, '0')}`,
        }))
        : [];
    return {
        name: instance?.name ?? 'New hospital',
        wards: isDemoInstance ? [
            {
                id: `${instanceId}-WARD-1`,
                name: 'Medical Ward',
                phase: 'Active',
                items: makeItems(instanceId === 'HC002' ? 'TRT-201' : 'TRT-101'),
            },
            {
                id: `${instanceId}-WARD-2`,
                name: 'Surgical Ward',
                phase: 'Active',
                items: makeItems(instanceId === 'HC002' ? 'TRT-202' : 'TRT-102').slice(0, 2),
            },
        ] : [],
    };
}
