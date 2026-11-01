import { describe, expect, it } from 'vitest';
import type { ApiContainer, ApiItem } from './workflow';
import {
    derivePhase,
    mapAssignment,
    mapEducation,
    mapHospital,
    mapWard,
    normalizeState,
    phaseOf,
} from './mapWorkflow';

function item(state: string, extra: Partial<ApiItem> = {}): ApiItem {
    return { id: 'T1', type: 'Item', name: 'Patient', state, assignee: null, reviewer: null, ...extra };
}

function container(items: ApiItem[], phase?: string): ApiContainer {
    return { id: 'C1', type: 'ward', name: 'Ward', phase, children: items };
}

describe('normalizeState', () => {
    it('maps every state name the backend sends', () => {
        expect(normalizeState('Available')).toBe('available');
        expect(normalizeState('Assigned')).toBe('assigned');
        expect(normalizeState('InProgress')).toBe('inprogress');
        expect(normalizeState('Completed')).toBe('completed');
        expect(normalizeState('Rejected')).toBe('rejected');
    });

    it('falls back to available for an unknown state', () => {
        expect(normalizeState('Mystery')).toBe('available');
    });
});

describe('derivePhase', () => {
    it('treats an empty or untouched container as Intake', () => {
        expect(derivePhase([])).toBe('Intake');
        expect(derivePhase([item('Available'), item('Available')])).toBe('Intake');
    });

    it('treats assigned or in-progress work as Active', () => {
        expect(derivePhase([item('Assigned'), item('Completed')])).toBe('Active');
        expect(derivePhase([item('InProgress')])).toBe('Active');
    });

    it('treats finished but unreviewed work, or a rejection, as Review', () => {
        expect(derivePhase([item('Completed'), item('Completed')])).toBe('Review');
        expect(derivePhase([item('Completed', { reviewer: 'R1' }), item('Rejected')])).toBe('Review');
    });

    it('treats fully reviewed work as Publish', () => {
        expect(derivePhase([
            item('Completed', { reviewer: 'R1' }),
            item('Completed', { reviewer: 'R2' }),
        ])).toBe('Publish');
    });
});

describe('phaseOf', () => {
    it('prefers the phase reported by the server, ignoring case', () => {
        expect(phaseOf(container([item('Available')], 'Review'))).toBe('Review');
        expect(phaseOf(container([item('Available')], 'publish'))).toBe('Publish');
    });

    it('derives the phase when the server sends none or an unknown one', () => {
        expect(phaseOf(container([item('Assigned')]))).toBe('Active');
        expect(phaseOf(container([item('Available')], 'Nonsense'))).toBe('Intake');
    });
});

describe('healthcare mapping', () => {
    it('reads the recorded result as severity and fills safe defaults', () => {
        const ward = mapWard(container([
            item('Completed', { assignee: 'D1', reviewer: 'N1', result: 0, diagnosis: 'Flu', age: 41 }),
            item('Available'),
        ], 'Review'));
        expect(ward.phase).toBe('Review');
        expect(ward.items[0]).toMatchObject({
            assignee: 'D1',
            reviewer: 'N1',
            severity: 0,
            diagnosis: 'Flu',
            age: 41,
            state: 'completed',
        });
        expect(ward.items[1]).toMatchObject({ assignee: '', reviewer: '', diagnosis: '', severity: undefined });
        expect(ward.items[1].age).toBeUndefined();
    });

    it('maps the hospital name and every ward', () => {
        const mapped = mapHospital({
            id: 'h',
            type: 'hospital',
            name: 'City Hospital',
            children: [container([item('Available')]), { ...container([]), id: 'C2', name: 'ICU' }],
        });
        expect(mapped.name).toBe('City Hospital');
        expect(mapped.wards.map((ward) => ward.id)).toEqual(['C1', 'C2']);
    });
});

describe('education mapping', () => {
    it('reads the recorded result as the mark', () => {
        const assignment = mapAssignment(container([
            item('Completed', { name: 'Student A', result: 78, reviewer: 'Dr Dlamini' }),
            item('Assigned', { name: 'Student B', assignee: 'Alice' }),
        ], 'Active'));
        expect(assignment.phase).toBe('Active');
        expect(assignment.items[0]).toMatchObject({ name: 'Student A', mark: 78, reviewer: 'Dr Dlamini' });
        expect(assignment.items[1]).toMatchObject({ assignee: 'Alice', reviewer: '', mark: undefined });
    });

    it('maps the course name and every assignment', () => {
        const mapped = mapEducation({
            id: 'root',
            type: 'course',
            name: 'COS 214',
            children: [container([]), { ...container([]), id: 'C2', name: 'Test 1' }],
        });
        expect(mapped.name).toBe('COS 214');
        expect(mapped.assignments.map((assignment) => assignment.id)).toEqual(['C1', 'C2']);
    });
});
