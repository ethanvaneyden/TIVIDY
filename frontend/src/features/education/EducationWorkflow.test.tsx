import { render, screen, waitFor } from '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { describe, expect, it } from 'vitest';
import { container, installFakeApi, item, root } from '../../test/fakeApi';
import EducationWorkflow from './EducationWorkflow';

const ID = 'edu_0_COS 214';
const BASE = `/api/education/${ID}`;

function course(phase: string, items: unknown[]) {
    return root('course', 'COS 214', [container('assignment', 'A1', 'Assignment 1', phase, items)]);
}

describe('EducationWorkflow', () => {
    it('shows the assignment, its phase and its submissions', async () => {
        installFakeApi((call) => {
            if (call.path === `${BASE}/state`) {
                return { json: course('Active', [item('SUB-A', 'Student A', 'Assigned', { assignee: 'Alice' })]) };
            }
            if (call.path === `${BASE}/history`) return { json: [] };
            return undefined;
        });
        render(<EducationWorkflow instanceId={ID} />);
        expect(await screen.findByText('Student A')).toBeTruthy();
        expect(screen.getByText('Assigned to Alice')).toBeTruthy();
        expect(screen.getByText('Active')).toBeTruthy();
        expect(screen.getByRole('button', { name: 'Start' })).toBeTruthy();
        expect(screen.queryByRole('button', { name: 'Start marking' })).toBeNull();
    });

    it('adds a submission through the documented endpoint', async () => {
        const calls = installFakeApi((call) => {
            if (call.path === `${BASE}/state`) return { json: course('Intake', []) };
            if (call.path === `${BASE}/history`) return { json: [] };
            if (call.path === `${BASE}/containers/A1/submissions`) return { json: { result: 'Success' } };
            return undefined;
        });
        render(<EducationWorkflow instanceId={ID} />);

        await userEvent.click(await screen.findByRole('button', { name: '+ Add submission' }));
        await userEvent.type(await screen.findByLabelText('Student name'), 'Student Z');
        await userEvent.click(screen.getByRole('button', { name: 'Add submission' }));

        await waitFor(() => {
            expect(calls.find((call) => call.path === `${BASE}/containers/A1/submissions`)?.body).toEqual({ studentName: 'Student Z' });
        });
    });

    it('adds an assignment through the submit route', async () => {
        const calls = installFakeApi((call) => {
            if (call.path === `${BASE}/state`) return { json: course('Intake', []) };
            if (call.path === `${BASE}/history`) return { json: [] };
            if (call.path === `${BASE}/submit`) return { json: { result: 'Success' } };
            return undefined;
        });
        render(<EducationWorkflow instanceId={ID} />);

        await userEvent.click(await screen.findByRole('button', { name: '+ Add assignment / test' }));
        await userEvent.type(await screen.findByLabelText('Assignment or test name'), 'Test 2');
        await userEvent.click(screen.getByRole('button', { name: 'Add assignment / test', hidden: false }));

        await waitFor(() => {
            expect(calls.find((call) => call.path === `${BASE}/submit`)?.body).toEqual({ containerName: 'Test 2' });
        });
    });

    it('starts marking only when the assignment has submissions', async () => {
        const calls = installFakeApi((call) => {
            if (call.path === `${BASE}/state`) return { json: course('Intake', [item('SUB-A', 'Student A', 'Available')]) };
            if (call.path === `${BASE}/history`) return { json: [] };
            if (call.path === `${BASE}/containers/A1/open`) return { json: { result: 'Success' } };
            return undefined;
        });
        render(<EducationWorkflow instanceId={ID} />);
        await userEvent.click(await screen.findByRole('button', { name: 'Start marking' }));
        await waitFor(() => {
            expect(calls.some((call) => call.method === 'POST' && call.path === `${BASE}/containers/A1/open`)).toBe(true);
        });
    });

    it('sends a mark of up to 100 and rejects larger values', async () => {
        const calls = installFakeApi((call) => {
            if (call.path === `${BASE}/state`) return { json: course('Active', [item('SUB-A', 'Student A', 'InProgress', { assignee: 'Alice' })]) };
            if (call.path === `${BASE}/history`) return { json: [] };
            if (call.path === `${BASE}/items/SUB-A/complete`) return { json: { result: 'Success' } };
            return undefined;
        });
        render(<EducationWorkflow instanceId={ID} />);

        await userEvent.click(await screen.findByRole('button', { name: 'Complete' }));
        const input = await screen.findByLabelText(/Mark/);
        await userEvent.type(input, '101');
        expect((screen.getAllByRole('button', { name: 'Complete' }).pop() as HTMLButtonElement).disabled).toBe(true);

        await userEvent.clear(input);
        await userEvent.type(input, '78');
        await userEvent.click(screen.getAllByRole('button', { name: 'Complete' }).pop() as HTMLButtonElement);

        await waitFor(() => {
            expect(calls.find((call) => call.path === `${BASE}/items/SUB-A/complete`)?.body).toEqual({ value: 78 });
        });
    });

    it('lets the lecturer keep a rejected mark', async () => {
        const calls = installFakeApi((call) => {
            if (call.path === `${BASE}/state`) return { json: course('Review', [item('SUB-G', 'Student G', 'Rejected', { result: 35 })]) };
            if (call.path === `${BASE}/history`) return { json: [] };
            if (call.path === `${BASE}/items/SUB-G/override`) return { json: { result: 'Success' } };
            return undefined;
        });
        render(<EducationWorkflow instanceId={ID} />);

        await userEvent.click(await screen.findByRole('button', { name: 'Resolve' }));
        await userEvent.click(await screen.findByLabelText('Keep the recorded value'));
        await userEvent.click(screen.getAllByRole('button', { name: 'Resolve' }).pop() as HTMLButtonElement);

        await waitFor(() => {
            expect(calls.find((call) => call.path === `${BASE}/items/SUB-G/override`)?.body).toEqual({ value: 0, accept: 1 });
        });
    });

    it('shows a retry when the instance cannot be loaded', async () => {
        installFakeApi(() => undefined);
        render(<EducationWorkflow instanceId={ID} />);
        expect(await screen.findByText('Could not load this instance')).toBeTruthy();
    });
});
