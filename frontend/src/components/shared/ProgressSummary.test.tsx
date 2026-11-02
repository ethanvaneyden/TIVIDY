import { render, screen } from '@testing-library/react';
import { describe, expect, it } from 'vitest';
import ProgressSummary from './ProgressSummary';

describe('ProgressSummary', () => {
    it('shows overall progress and a chip per lifecycle state', () => {
        render(
            <ProgressSummary
                itemLabel="submissions"
                report={{
                    totalItems: 8,
                    completedItems: 2,
                    percentComplete: 25,
                    states: { Assigned: 6, Completed: 2 },
                    containers: [],
                }}
            />,
        );
        expect(screen.getByText('2 of 8 submissions completed')).toBeInTheDocument();
        expect(screen.getByText('25%')).toBeInTheDocument();
        expect(screen.getByText('Assigned')).toBeInTheDocument();
        expect(screen.getByText('Completed')).toBeInTheDocument();
    });

    it('renders nothing before the report has loaded', () => {
        const { container } = render(<ProgressSummary itemLabel="patients" />);
        expect(container).toBeEmptyDOMElement();
    });
});
