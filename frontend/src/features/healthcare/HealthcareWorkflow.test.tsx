import { render, screen, waitFor } from '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { describe, expect, it } from 'vitest';
import { container, installFakeApi, item, root } from '../../test/fakeApi';
import HealthcareWorkflow from './HealthcareWorkflow';

const ID = 'hc_0_City Hospital';
const BASE = `/api/healthcare/${ID}`;

function ward(phase: string, items: unknown[]) {
    return root('hospital', 'City Hospital', [container('ward', 'W1', 'General Ward', phase, items)]);
}

describe('HealthcareWorkflow', () => {
    it('shows the ward from the server and opens it', async () => {
        let phase = 'Intake';
        const calls = installFakeApi((call) => {
            if (call.path === `${BASE}/state`) {
                const state = phase === 'Intake' ? 'Available' : 'Assigned';
                return { json: ward(phase, [item('ITEM-1', 'Karabo Sithole', state, phase === 'Intake' ? {} : { assignee: 'Doctor-1' })]) };
            }
            if (call.path === `${BASE}/history`) return { json: ['ITEM-1 added'] };
            if (call.path === `${BASE}/containers/W1/open`) {
                phase = 'Active';
                return { json: { result: 'Success' } };
            }
            return undefined;
        });

        render(<HealthcareWorkflow instanceId={ID} />);

        expect(await screen.findByText('Karabo Sithole')).toBeTruthy();
        expect(screen.getByText('ITEM-1 added')).toBeTruthy();
        expect(screen.queryByRole('button', { name: 'Start' })).toBeNull();

        await userEvent.click(screen.getByRole('button', { name: 'Open ward' }));

        expect(await screen.findByRole('button', { name: 'Start' })).toBeTruthy();
        expect(calls.some((call) => call.method === 'POST' && call.path === `${BASE}/containers/W1/open`)).toBe(true);
        expect(screen.queryByRole('button', { name: 'Open ward' })).toBeNull();
    });

    it('disables Open ward while the ward has no patients', async () => {
        installFakeApi((call) => {
            if (call.path === `${BASE}/state`) return { json: ward('Intake', []) };
            if (call.path === `${BASE}/history`) return { json: [] };
            return undefined;
        });
        render(<HealthcareWorkflow instanceId={ID} />);
        const button = await screen.findByRole('button', { name: 'Open ward' });
        expect((button as HTMLButtonElement).disabled).toBe(true);
    });

    it('shows the server error when an action is not allowed', async () => {
        installFakeApi((call) => {
            if (call.path === `${BASE}/state`) return { json: ward('Active', [item('ITEM-1', 'Anele Zulu', 'Assigned')]) };
            if (call.path === `${BASE}/history`) return { json: [] };
            if (call.path === `${BASE}/items/ITEM-1/start`) return { status: 409, json: { result: 'InvalidTransition' } };
            return undefined;
        });
        render(<HealthcareWorkflow instanceId={ID} />);
        await userEvent.click(await screen.findByRole('button', { name: 'Start' }));
        expect(await screen.findByText(/not allowed in the current state/)).toBeTruthy();
    });

    it('records a severity and sends it as the completion value', async () => {
        const calls = installFakeApi((call) => {
            if (call.path === `${BASE}/state`) return { json: ward('Active', [item('ITEM-1', 'Anele Zulu', 'InProgress', { assignee: 'Doctor-1' })]) };
            if (call.path === `${BASE}/history`) return { json: [] };
            if (call.path === `${BASE}/items/ITEM-1/complete`) return { json: { result: 'Success' } };
            return undefined;
        });
        render(<HealthcareWorkflow instanceId={ID} />);

        await userEvent.click(await screen.findByRole('button', { name: 'Complete' }));

        const input = await screen.findByLabelText(/Severity/);
        await userEvent.type(input, '11');
        expect((screen.getAllByRole('button', { name: 'Complete' }).pop() as HTMLButtonElement).disabled).toBe(true);

        await userEvent.clear(input);
        await userEvent.type(input, '9');
        await userEvent.click(screen.getAllByRole('button', { name: 'Complete' }).pop() as HTMLButtonElement);

        await waitFor(() => {
            expect(calls.find((call) => call.path === `${BASE}/items/ITEM-1/complete`)?.body).toEqual({ value: 9 });
        });
    });

    it('lets the chief resolve a rejected treatment with a new severity', async () => {
        const calls = installFakeApi((call) => {
            if (call.path === `${BASE}/state`) {
                return { json: ward('Review', [item('ITEM-2', 'Thabo Molefe', 'Rejected', { result: 0 })]) };
            }
            if (call.path === `${BASE}/history`) return { json: [] };
            if (call.path === `${BASE}/items/ITEM-2/override`) return { json: { result: 'Success' } };
            return undefined;
        });
        render(<HealthcareWorkflow instanceId={ID} />);

        await userEvent.click(await screen.findByRole('button', { name: 'Resolve' }));
        await userEvent.type(await screen.findByLabelText(/New severity/), '5');
        await userEvent.click(screen.getAllByRole('button', { name: 'Resolve' }).pop() as HTMLButtonElement);

        await waitFor(() => {
            expect(calls.find((call) => call.path === `${BASE}/items/ITEM-2/override`)?.body).toEqual({ value: 5, accept: 0 });
        });
    });

    it('offers batch approval only in the Review phase', async () => {
        const calls = installFakeApi((call) => {
            if (call.path === `${BASE}/state`) return { json: ward('Review', [item('ITEM-1', 'Anele Zulu', 'Completed', { result: 5 })]) };
            if (call.path === `${BASE}/history`) return { json: [] };
            if (call.path === `${BASE}/containers/W1/approve`) return { json: { result: 'Success' } };
            return undefined;
        });
        render(<HealthcareWorkflow instanceId={ID} />);
        await userEvent.click(await screen.findByRole('button', { name: 'Approve batch' }));
        await waitFor(() => {
            expect(calls.some((call) => call.method === 'POST' && call.path === `${BASE}/containers/W1/approve`)).toBe(true);
        });
        expect(screen.queryByRole('button', { name: 'Open ward' })).toBeNull();
    });

    it('shows a retry when the instance cannot be loaded', async () => {
        installFakeApi(() => undefined);
        render(<HealthcareWorkflow instanceId={ID} />);
        expect(await screen.findByText('Could not load this hospital')).toBeTruthy();
        expect(screen.getByRole('button', { name: 'Try again' })).toBeTruthy();
    });
});
