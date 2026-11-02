import { render, screen } from '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { describe, expect, it, vi } from 'vitest';
import type { Assignment } from '../../types/types';
import ContainerCard from './ContainerCard';

function assignment(phase: Assignment['phase'], withItems = true): Assignment {
    return {
        id: 'A1',
        name: 'Assignment 1',
        phase,
        items: withItems
            ? [{ id: 'SUB-A', name: 'Student A', state: 'available', assignee: '', reviewer: '' }]
            : [],
    };
}

function renderCard(phase: Assignment['phase'], withItems = true) {
    const handlers = { onView: vi.fn(), onAdd: vi.fn(), onApprove: vi.fn(), onOpen: vi.fn() };
    render(
        <ContainerCard
            assignment={assignment(phase, withItems)}
            onViewSubmission={handlers.onView}
            onAddSubmission={handlers.onAdd}
            onApprove={handlers.onApprove}
            onOpen={handlers.onOpen}
        />,
    );
    return handlers;
}

describe('ContainerCard actions follow the phase', () => {
    it('offers Open (and adding) in Intake, and calls onOpen', async () => {
        const handlers = renderCard('Intake');
        await userEvent.click(screen.getByRole('button', { name: /open for marking/i }));
        expect(handlers.onOpen).toHaveBeenCalledTimes(1);
        expect(screen.getByRole('button', { name: /add submission/i })).toBeEnabled();
        expect(screen.queryByRole('button', { name: /approve all/i })).not.toBeInTheDocument();
    });

    it('does not offer Open for an empty assignment', () => {
        renderCard('Intake', false);
        expect(screen.queryByRole('button', { name: /open for marking/i })).not.toBeInTheDocument();
    });

    it('locks adding once Active and does not offer Open or Approve', () => {
        renderCard('Active');
        expect(screen.getByRole('button', { name: /add submission/i })).toBeDisabled();
        expect(screen.queryByRole('button', { name: /open for marking/i })).not.toBeInTheDocument();
        expect(screen.queryByRole('button', { name: /approve all/i })).not.toBeInTheDocument();
    });

    it('offers Approve all only in Review', async () => {
        const handlers = renderCard('Review');
        await userEvent.click(screen.getByRole('button', { name: /approve all/i }));
        expect(handlers.onApprove).toHaveBeenCalledTimes(1);
    });
});
