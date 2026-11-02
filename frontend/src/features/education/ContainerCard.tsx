import { Button, Card } from 'react-bootstrap';
import type { Assignment, Submission } from '../../types/types';
import SubmissionRow from './SubmissionRow';
import '../../styles/ContainerCard.css';

interface ContainerCardProps {
    assignment: Assignment;
    onViewSubmission: (submission: Submission) => void;
    onAddSubmission: () => void;
    onApprove: () => void;
    onOpen: () => void;
    busy?: boolean;
}

function ContainerCard({ assignment, onViewSubmission, onAddSubmission, onApprove, onOpen, busy = false }: ContainerCardProps) {
    const canApprove = assignment.phase === 'Review';
    const canOpen = assignment.phase === 'Intake' && assignment.items.length > 0;
    const canAdd = assignment.phase === 'Intake';

    return (
        <Card className="container-card">
            <Card.Header className="container-card-header">
                <div>
                    <span className="workflow-section-label">ASSIGNMENT / TEST</span>
                    <h2>{assignment.name}</h2>
                </div>
                <div className="d-flex gap-2 align-items-center">
                    <span className={`phase-pill phase-${assignment.phase.toLowerCase()}`}>{assignment.phase}</span>
                    {canOpen && (
                        <Button variant="success" size="sm" onClick={onOpen} disabled={busy}>
                            Open for marking
                        </Button>
                    )}
                    {canApprove && (
                        <Button variant="primary" size="sm" onClick={onApprove} disabled={busy}>
                            Approve all
                        </Button>
                    )}
                </div>
            </Card.Header>
            <Card.Body className="container-card-body">
                <div className="assignment-actions">
                    <Button variant="outline-primary" size="sm" onClick={onAddSubmission} disabled={!canAdd || busy}>
                        + Add submission
                    </Button>
                </div>
                {assignment.phase === 'Intake' && assignment.items.length > 0 && (
                    <p className="container-hint">
                        Submissions can be added until the assignment is opened. Opening assigns a marker to every submission.
                    </p>
                )}
                {assignment.phase === 'Review' && (
                    <p className="container-hint">
                        All submissions are marked. Approve all runs each mark through the approval chain.
                    </p>
                )}
                <div className="submission-list">
                    {assignment.items.length > 0 ? assignment.items.map((item) => (
                        <SubmissionRow
                            key={item.id}
                            submission={item}
                            onView={() => onViewSubmission(item)}
                        />
                    )) : (
                        <div className="submission-empty">
                            No submissions are available.
                        </div>
                    )}
                </div>
            </Card.Body>
        </Card>
    );
}

export default ContainerCard;