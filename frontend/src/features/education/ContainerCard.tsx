import { Button, Card } from 'react-bootstrap';
import type { Assignment, Submission } from '../../types/types';
import SubmissionRow from './SubmissionRow';
import '../../styles/ContainerCard.css';

interface ContainerCardProps {
    assignment: Assignment;
    busy: boolean;
    onViewSubmission: (submission: Submission) => void;
    onAddSubmission: () => void;
    onOpen: () => void;
    onApprove: () => void;
    onStartSubmission: (submission: Submission) => void;
    onCompleteSubmission: (submission: Submission) => void;
    onResolveSubmission: (submission: Submission) => void;
}

function ContainerCard({
    assignment,
    busy,
    onViewSubmission,
    onAddSubmission,
    onOpen,
    onApprove,
    onStartSubmission,
    onCompleteSubmission,
    onResolveSubmission,
}: ContainerCardProps) {
    const inIntake = assignment.phase === 'Intake';

    return (
        <Card className="container-card">
            <Card.Header className="container-card-header">
                <div>
                    <span className="workflow-section-label">ASSIGNMENT / TEST</span>
                    <h2>{assignment.name}</h2>
                </div>
                <span className={`phase-pill phase-${assignment.phase.toLowerCase()}`}>{assignment.phase}</span>
            </Card.Header>
            <Card.Body className="container-card-body">
                <div className="assignment-actions">
                    {inIntake && (
                        <>
                            <Button variant="outline-primary" size="sm" onClick={onAddSubmission} disabled={busy}>
                                + Add submission
                            </Button>
                            <Button
                                variant="primary"
                                size="sm"
                                onClick={onOpen}
                                disabled={busy || assignment.items.length === 0}
                                title={assignment.items.length === 0 ? 'Add at least one submission first' : undefined}
                            >
                                Start marking
                            </Button>
                        </>
                    )}
                    {assignment.phase === 'Review' && (
                        <Button variant="success" size="sm" onClick={onApprove} disabled={busy}>
                            Approve batch
                        </Button>
                    )}
                </div>
                <div className="submission-list">
                    {assignment.items.length > 0 ? assignment.items.map((item) => (
                        <SubmissionRow
                            key={item.id}
                            submission={item}
                            phase={assignment.phase}
                            busy={busy}
                            onView={() => onViewSubmission(item)}
                            onStart={() => onStartSubmission(item)}
                            onComplete={() => onCompleteSubmission(item)}
                            onResolve={() => onResolveSubmission(item)}
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
