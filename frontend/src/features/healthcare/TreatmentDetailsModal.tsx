import { Button, Modal } from 'react-bootstrap';
import { useState } from 'react';
import CompleteItemModal from '../../components/modals/CompleteItemModal';
import OverrideItemModal from '../../components/modals/OverrideItemModal';
import type { Treatment } from '../../types/types';

interface TreatmentDetailsModalProps {
    treatment?: Treatment;
    onHide: () => void;
    busy?: boolean;
    onStart?: (itemId: string) => Promise<boolean>;
    onComplete?: (itemId: string, severity: number) => Promise<boolean>;
    onOverride?: (itemId: string, severity: number, accept: boolean) => Promise<boolean>;
}

function TreatmentDetailsModal({
    treatment,
    onHide,
    busy = false,
    onStart,
    onComplete,
    onOverride,
}: TreatmentDetailsModalProps) {
    const [showCompleteModal, setShowCompleteModal] = useState(false);
    const [showOverrideModal, setShowOverrideModal] = useState(false);

    async function handleStart() {
        if (treatment && onStart) {
            await onStart(treatment.id);
        }
    }

    async function handleComplete(value: number): Promise<boolean> {
        if (!treatment || !onComplete) return false;
        return onComplete(treatment.id, value);
    }

    async function handleOverride(value: number, accept: boolean): Promise<boolean> {
        if (!treatment || !onOverride) return false;
        return onOverride(treatment.id, value, accept);
    }

    const canStart = treatment?.state === 'assigned';
    const waitingForOpen = treatment?.state === 'available';
    const canComplete = treatment?.state === 'inprogress';
    const canOverride = treatment?.state === 'rejected';

    return (
        <>
            <Modal show={Boolean(treatment) && !showCompleteModal && !showOverrideModal} onHide={onHide} centered>
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
                            {waitingForOpen && (
                                <p className="detail-hint">No doctor yet. Open the ward to assign doctors first.</p>
                            )}
                            {treatment.state === 'rejected' && treatment.rejectionReason && (
                                <p className="detail-rejection">{treatment.rejectionReason}</p>
                            )}
                        </div>
                    )}
                </Modal.Body>
                <Modal.Footer>
                    {canStart && onStart && (
                        <Button variant="primary" onClick={handleStart} disabled={busy}>
                            {busy ? 'Starting…' : 'Start'}
                        </Button>
                    )}
                    {canComplete && onComplete && (
                        <Button variant="success" onClick={() => setShowCompleteModal(true)} disabled={busy}>
                            Complete
                        </Button>
                    )}
                    {canOverride && onOverride && (
                        <Button variant="warning" onClick={() => setShowOverrideModal(true)} disabled={busy}>
                            Override
                        </Button>
                    )}
                    <Button variant="secondary" onClick={onHide}>Close</Button>
                </Modal.Footer>
            </Modal>

            <CompleteItemModal
                itemName={showCompleteModal ? treatment?.name : undefined}
                title="Record severity"
                label="Severity"
                min={0}
                max={10}
                hint="Severity 1-3 is approved by the triage nurse, 4-6 by the admitting doctor, 7-10 by the chief of medicine. 0 cannot be approved and is sent to the chief of medicine."
                busy={busy}
                primaryClassName="healthcare-primary-button"
                onHide={() => setShowCompleteModal(false)}
                onSubmit={handleComplete}
            />

            <OverrideItemModal
                itemName={showOverrideModal ? treatment?.name : undefined}
                currentValue={treatment?.severity}
                title="Override rejected treatment"
                label="Severity"
                min={1}
                max={10}
                deciderLabel="chief of medicine"
                busy={busy}
                primaryClassName="healthcare-primary-button"
                onHide={() => setShowOverrideModal(false)}
                onSubmit={handleOverride}
            />
        </>
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