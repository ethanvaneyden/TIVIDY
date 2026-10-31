import { Button, Col, Row } from 'react-bootstrap';
import type { Submission, WorkflowPhase } from '../../types/types';
import '../../styles/SubmissionRow.css';

interface SubmissionRowProps {
    submission: Submission;
    phase: WorkflowPhase;
    busy: boolean;
    onView: () => void;
    onStart: () => void;
    onComplete: () => void;
    onResolve: () => void;
}

const stateLabels: Record<Submission['state'], string> = {
    available: 'Available',
    assigned: 'Assigned',
    inprogress: 'In progress',
    completed: 'Completed',
    rejected: 'Rejected',
};

function SubmissionRow({ submission, phase, busy, onView, onStart, onComplete, onResolve }: SubmissionRowProps) {
    const { name, state, assignee, reviewer, mark } = submission;
    const canStart = phase === 'Active' && state === 'assigned';
    const canComplete = phase === 'Active' && state === 'inprogress';
    const canResolve = phase === 'Review' && state === 'rejected';

    return (
        <Row className={`submission-row submission-state-${state} align-items-center g-0`}>
            <Col xs={12} md={4} className="submission-person">
                <span className="submission-avatar" aria-hidden="true">{getInitials(name)}</span>
                <span className="submission-person-copy">
                    <strong>{name}</strong>
                    <span>{assignee ? `Assigned to ${assignee}` : 'No worker assigned'}</span>
                </span>
            </Col>
            <Col xs={6} md={3} className="submission-status-column">
                <span className={`submission-status status-${state}`}>
                    <span className="submission-status-dot" />
                    {stateLabels[state]}
                </span>
                {mark !== undefined && <span className="submission-mark">{mark}/100</span>}
            </Col>
            <Col xs={6} md={2} className="submission-reviewer">
                <span className="submission-reviewer-label">Reviewer</span>
                <span>{reviewer || '—'}</span>
            </Col>
            <Col xs={12} md={3} className="submission-action">
                <div className="row-actions">
                    {canStart && (
                        <Button type="button" variant="primary" size="sm" onClick={onStart} disabled={busy}>
                            Start
                        </Button>
                    )}
                    {canComplete && (
                        <Button type="button" variant="success" size="sm" onClick={onComplete} disabled={busy}>
                            Complete
                        </Button>
                    )}
                    {canResolve && (
                        <Button type="button" variant="warning" size="sm" onClick={onResolve} disabled={busy}>
                            Resolve
                        </Button>
                    )}
                    <Button type="button" variant="outline-secondary" size="sm" onClick={onView}>
                        Details
                    </Button>
                </div>
            </Col>
        </Row>
    );
}

function getInitials(name: string): string {
    return name
        .split(' ')
        .slice(0, 2)
        .map((part) => part[0] ?? '')
        .join('')
        .toUpperCase();
}

export default SubmissionRow;
