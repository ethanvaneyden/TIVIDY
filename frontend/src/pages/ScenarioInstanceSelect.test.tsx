import { render, screen, waitFor } from '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { MemoryRouter, Route, Routes } from 'react-router';
import { describe, expect, it } from 'vitest';
import { installFakeApi } from '../test/fakeApi';
import ScenarioInstanceSelect from './ScenarioInstanceSelect';

function renderPicker(path: string) {
    return render(
        <MemoryRouter initialEntries={[path]}>
            <Routes>
                <Route path="/:scenario/instances" element={<ScenarioInstanceSelect />} />
                <Route path="/:scenario/:instanceId" element={<p>Opened workflow</p>} />
            </Routes>
        </MemoryRouter>,
    );
}

describe('ScenarioInstanceSelect', () => {
    it('lists the instances the server reports, named from their ids', async () => {
        installFakeApi((call) => {
            if (call.path === '/api/education/instances') return { json: ['edu_0_COS 214', 'edu_1_COS 332'] };
            return undefined;
        });
        renderPicker('/education/instances');
        expect(await screen.findByText('COS 214')).toBeTruthy();
        expect(screen.getByText('COS 332')).toBeTruthy();
        expect(screen.getByText('edu_0_COS 214')).toBeTruthy();
    });

    it('asks the healthcare route for healthcare instances', async () => {
        const calls = installFakeApi((call) => {
            if (call.path === '/api/healthcare/instances') return { json: ['hc_0_City Hospital'] };
            return undefined;
        });
        renderPicker('/healthcare/instances');
        expect(await screen.findByText('City Hospital')).toBeTruthy();
        expect(calls[0].path).toBe('/api/healthcare/instances');
    });

    it('shows the empty state and the failure state', async () => {
        installFakeApi((call) => (call.path === '/api/education/instances' ? { json: [] } : undefined));
        renderPicker('/education/instances');
        expect(await screen.findByText('No instances yet')).toBeTruthy();
    });

    it('shows a retry when the server cannot be listed', async () => {
        installFakeApi(() => undefined);
        renderPicker('/education/instances');
        expect(await screen.findByText('Could not load instances')).toBeTruthy();
    });

    it('creates an instance and opens it', async () => {
        const calls = installFakeApi((call) => {
            if (call.path === '/api/education/instances') return { json: [] };
            if (call.path === '/api/education/start') return { json: { instanceId: 'edu_0_New' } };
            return undefined;
        });
        renderPicker('/education/instances');

        await userEvent.click(await screen.findByRole('button', { name: /Create new instance/ }));
        await userEvent.type(await screen.findByLabelText('Instance name'), 'New');
        await userEvent.click(screen.getByRole('button', { name: 'Create instance' }));

        expect(await screen.findByText('Opened workflow')).toBeTruthy();
        await waitFor(() => {
            expect(calls.find((call) => call.path === '/api/education/start')?.body).toEqual({ name: 'New' });
        });
    });

    it('hides delete buttons while the server has no delete route', async () => {
        installFakeApi((call) => (call.path === '/api/education/instances' ? { json: ['edu_0_COS 214'] } : undefined));
        renderPicker('/education/instances');
        await screen.findByText('COS 214');
        expect(screen.queryByLabelText(/Delete instance/)).toBeNull();
    });
});
