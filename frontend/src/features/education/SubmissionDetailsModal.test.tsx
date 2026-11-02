import { render, screen } from '@testing-library/react';
import { describe, expect, it, vi } from 'vitest';
import type { Submission } from '../../types/types';
import SubmissionDetailsModal from './SubmissionDetailsModal';

function open(state: Submission['state'], extra: Partial<Submission> = {}) {
    render(
        <SubmissionDetailsModal
            submission={{ id: 'SUB-A', name: 'Student A', state, assignee: 'Alice', reviewer: '', ...extra }}
            onHide={vi.fn()}
            onStart={vi.fn()}
            onComplete={vi.fn()}
            onOverride={vi.fn()}
        />,
    );
}

describe('SubmissionDetailsModal', () => {
    it('only lets an assigned submission be started', () => {
        open('assigned');
        expect(screen.getByRole('button', { name: 'Start' })).toBeInTheDocument();
    });

    it('tells the user to open the assignment when nobody is assigned yet', () => {
        open('available', { assignee: '' });
        expect(screen.queryByRole('button', { name: 'Start' })).not.toBeInTheDocument();
        expect(screen.getByText(/open the assignment/i)).toBeInTheDocument();
    });

    it('offers Complete for work in progress and Override with the reason for a rejection', () => {
        open('inprogress');
        expect(screen.getByRole('button', { name: 'Complete' })).toBeInTheDocument();
    });

    it('shows why a submission was rejected', () => {
        open('rejected', { mark: 35, rejectionReason: 'Mark 35 cannot be auto-approved' });
        expect(screen.getByText('Mark 35 cannot be auto-approved')).toBeInTheDocument();
        expect(screen.getByRole('button', { name: 'Override' })).toBeInTheDocument();
    });
});
