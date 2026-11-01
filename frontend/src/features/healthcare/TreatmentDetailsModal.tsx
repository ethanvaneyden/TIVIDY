import { Button, Modal } from 'react-bootstrap';
import type { Treatment } from '../../types/types';

interface TreatmentDetailsModalProps {
    treatment?: Treatment;
    onHide: () => void;
}

function TreatmentDetailsModal({ treatment, onHide }: TreatmentDetailsModalProps) {
    return (
        <Modal show={Boolean(treatment)} onHide={onHide} centered>
            <Modal.Header closeButton>
                <Modal.Title>Treatment details</Modal.Title>
            </Modal.Header>
            <Modal.Body>
                {treatment && (
                    <div className="submission-detail">
                        <span className="submission-detail-id">{treatment.id}</span>
                        <h3>{treatment.name}</h3>
                        <dl>
                            <div><dt>Age</dt><dd>{treatment.age ?? 'Not recorded'}</dd></div>
                            <div><dt>Diagnosis</dt><dd>{treatment.diagnosis || 'Not recorded'}</dd></div>
                            <div><dt>Severity</dt><dd>{treatment.severity ?? 'Not recorded'}</dd></div>
                            <div><dt>Status</dt><dd>{formatState(treatment.state)}</dd></div>
                            <div><dt>Assigned worker</dt><dd>{treatment.assignee || 'Not assigned'}</dd></div>
                            <div><dt>Reviewer</dt><dd>{treatment.reviewer || 'Not reviewed'}</dd></div>
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

function formatState(state: Treatment['state']): string {
    return ({
        available: 'Available',
        assigned: 'Assigned',
        inprogress: 'In progress',
        completed: 'Completed',
        rejected: 'Rejected',
    })[state];
}

export default TreatmentDetailsModal;
