import { Button, Card } from 'react-bootstrap';
import type { Assignment, Submission } from '../../types/types';
import SubmissionRow from './SubmissionRow';
import '../../styles/ContainerCard.css';

// interface ContainerCardProps {
//     assignment: Assignment;
//     onViewSubmission: (submission: Submission) => void;
//     onAddSubmission: () => void;
// }

interface ContainerCardProps {
    assignment: Assignment;
    onViewSubmission: (submission: Submission) => void;
    onAddSubmission: () => void;
    onApprove: () => void;
}

// function ContainerCard({ assignment, onViewSubmission, onAddSubmission }: ContainerCardProps) {
//     return (
//         <Card className="container-card">
//             <Card.Header className="container-card-header">
//                 <div>
//                     <span className="workflow-section-label">ASSIGNMENT / TEST</span>
//                     <h2>{assignment.name}</h2>
//                 </div>
//                 <span className={`phase-pill phase-${assignment.phase.toLowerCase()}`}>{assignment.phase}</span>
//             </Card.Header>
//             <Card.Body className="container-card-body">
//                 <div className="assignment-actions">
//                     <Button variant="outline-primary" size="sm" onClick={onAddSubmission}>
//                         + Add submission
//                     </Button>
//                 </div>
//                 <div className="submission-list">
//                     {assignment.items.length > 0 ? assignment.items.map((item) => (
//                         <SubmissionRow
//                             key={item.id}
//                             submission={item}
//                             onView={() => onViewSubmission(item)}
//                         />
//                     )) : (
//                         <div className="submission-empty">
//                             No submissions are available.
//                         </div>
//                     )}
//                 </div>
//             </Card.Body>
//         </Card>
//     );
// }

function ContainerCard({ assignment, onViewSubmission, onAddSubmission, onApprove }: ContainerCardProps) {
    const allCompleted = assignment.items.length > 0 &&
        assignment.items.every((item) => item.state === 'completed' || item.state === 'rejected');
    const canApprove = assignment.phase === 'Review' || (assignment.phase === 'Active' && allCompleted);

    return (
        <Card className="container-card">
            <Card.Header className="container-card-header">
                <div>
                    <span className="workflow-section-label">ASSIGNMENT / TEST</span>
                    <h2>{assignment.name}</h2>
                </div>
                <div className="d-flex gap-2 align-items-center">
                    <span className={`phase-pill phase-${assignment.phase.toLowerCase()}`}>{assignment.phase}</span>
                    {canApprove && (
                        <Button variant="primary" size="sm" onClick={onApprove}>
                            Approve all
                        </Button>
                    )}
                </div>
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