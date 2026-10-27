import { Button, Card } from 'react-bootstrap';
import type { Assignment, Submission } from '../../types/types';
import SubmissionRow from './SubmissionRow';
import '../../styles/ContainerCard.css';

interface ContainerCardProps {
    assignment: Assignment;
    onViewSubmission: (submission: Submission) => void;
    onAddSubmission: () => void;
}

function ContainerCard({ assignment, onViewSubmission, onAddSubmission }: ContainerCardProps) {
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
                    <Button variant="outline-primary" size="sm" onClick={onAddSubmission}>
                        + Add submission
                    </Button>
                </div>
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
