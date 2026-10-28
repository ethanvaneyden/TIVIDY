import { Button, Modal } from 'react-bootstrap';
import type { Submission } from '../../types/types';

interface SubmissionDetailsModalProps {
    submission?: Submission;
    onHide: () => void;
}

function SubmissionDetailsModal({ submission, onHide }: SubmissionDetailsModalProps) {
    return (
        <Modal show={Boolean(submission)} onHide={onHide} centered>
            <Modal.Header closeButton>
                <Modal.Title>Submission details</Modal.Title>
            </Modal.Header>
            <Modal.Body>
                {submission && (
                    <div className="submission-detail">
                        <span className="submission-detail-id">{submission.id}</span>
                        <h3>{submission.name}</h3>
                        <dl>
                            <div><dt>Status</dt><dd>{formatState(submission.state)}</dd></div>
                            <div><dt>Mark</dt><dd>{submission.mark ?? 'Not recorded'}</dd></div>
                            <div><dt>Assigned worker</dt><dd>{submission.assignee || 'Not assigned'}</dd></div>
                            <div><dt>Reviewer</dt><dd>{submission.reviewer || 'Not reviewed'}</dd></div>
                        </dl>
                    </div>
                )}
            </Modal.Body>
            <Modal.Footer>
                <Button variant="secondary" onClick={onHide}>Close</Button>
            </Modal.Footer>
        </Modal>
    );
}

function formatState(state: Submission['state']): string {
    return ({
        available: 'Available',
        assigned: 'Assigned',
        inprogress: 'In progress',
        completed: 'Completed',
        rejected: 'Rejected',
    })[state];
}

export default SubmissionDetailsModal;
