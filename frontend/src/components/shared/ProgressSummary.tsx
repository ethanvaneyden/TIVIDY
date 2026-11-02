import { ProgressBar } from 'react-bootstrap';
import type { ProgressReport } from '../../types/types';

interface ProgressSummaryProps {
    report?: ProgressReport;
    itemLabel: string;
}

const STATE_LABELS: Record<string, string> = {
    Available: 'Available',
    Assigned: 'Assigned',
    InProgress: 'In progress',
    Completed: 'Completed',
    Rejected: 'Rejected',
};

/** Monitoring strip fed by the backend's ProgressReportVisitor. */
function ProgressSummary({ report, itemLabel }: ProgressSummaryProps) {
    if (!report) return null;

    return (
        <section className="progress-summary" aria-label="Workflow progress">
            <div className="progress-summary-head">
                <div>
                    <span className="workflow-section-label">PROGRESS</span>
                    <strong>
                        {report.completedItems} of {report.totalItems} {itemLabel} completed
                    </strong>
                </div>
                <span className="progress-summary-percent">{report.percentComplete}%</span>
            </div>
            <ProgressBar now={report.percentComplete} className="progress-summary-bar" aria-label="Overall completion" />
            <ul className="progress-summary-states">
                {Object.entries(report.states).map(([state, count]) => (
                    <li key={state} className={`progress-chip chip-${state.toLowerCase()}`}>
                        <span>{STATE_LABELS[state] ?? state}</span>
                        <strong>{count}</strong>
                    </li>
                ))}
            </ul>
        </section>
    );
}

export default ProgressSummary;
