import { Button, Modal } from 'react-bootstrap';
import { useState } from 'react';
import CompleteItemModal from '../../components/modals/CompleteItemModal';
import OverrideItemModal from '../../components/modals/OverrideItemModal';
import type { Submission } from '../../types/types';

// interface SubmissionDetailsModalProps {
//     submission?: Submission;
//     onHide: () => void;
// }

interface SubmissionDetailsModalProps {
    submission?: Submission;
    onHide: () => void;
    busy?: boolean;
    onStart?: (itemId: string) => Promise<boolean>;
    onComplete?: (itemId: string, mark: number) => Promise<boolean>;
    onOverride?: (itemId: string, mark: number, accept: boolean) => Promise<boolean>;
}

// function SubmissionDetailsModal({ submission, onHide }: SubmissionDetailsModalProps) {
//     return (
//         <Modal show={Boolean(submission)} onHide={onHide} centered>
//             <Modal.Header closeButton>
//                 <Modal.Title>Submission details</Modal.Title>
//             </Modal.Header>
//             <Modal.Body>
//                 {submission && (
//                     <div className="submission-detail">
//                         <span className="submission-detail-id">{submission.id}</span>
//                         <h3>{submission.name}</h3>
//                         <dl>
//                             <div><dt>Status</dt><dd>{formatState(submission.state)}</dd></div>
//                             <div><dt>Mark</dt><dd>{submission.mark ?? 'Not recorded'}</dd></div>
//                             <div><dt>Assigned worker</dt><dd>{submission.assignee || 'Not assigned'}</dd></div>
//                             <div><dt>Reviewer</dt><dd>{submission.reviewer || 'Not reviewed'}</dd></div>
//                         </dl>
//                     </div>
//                 )}
//             </Modal.Body>
//             <Modal.Footer>
//                 <Button variant="secondary" onClick={onHide}>Close</Button>
//             </Modal.Footer>
//         </Modal>
//     );
// }

function SubmissionDetailsModal({
    submission,
    onHide,
    busy = false,
    onStart,
    onComplete,
    onOverride,
}: SubmissionDetailsModalProps) {
    const [showCompleteModal, setShowCompleteModal] = useState(false);
    const [showOverrideModal, setShowOverrideModal] = useState(false);

    async function handleStart() {
        if (submission && onStart) {
            await onStart(submission.id);
        }
    }

    async function handleComplete(value: number): Promise<boolean> {
        if (!submission || !onComplete) return false;
        return onComplete(submission.id, value);
    }

    async function handleOverride(value: number, accept: boolean): Promise<boolean> {
        if (!submission || !onOverride) return false;
        return onOverride(submission.id, value, accept);
    }

    const canStart = submission?.state === 'available' || submission?.state === 'assigned';
    const canComplete = submission?.state === 'inprogress';
    const canOverride = submission?.state === 'rejected';

    return (
        <>
            <Modal show={Boolean(submission) && !showCompleteModal && !showOverrideModal} onHide={onHide} centered>
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
                itemName={showCompleteModal ? submission?.name : undefined}
                title="Complete submission"
                label="Mark"
                min={0}
                max={100}
                hint="Marks 40-49 are approved by the moderator, 50-70 by a TA, 71-100 by the lecturer. Marks below 40 are rejected and must be overridden."
                busy={busy}
                onHide={() => setShowCompleteModal(false)}
                onSubmit={handleComplete}
            />

            <OverrideItemModal
                itemName={showOverrideModal ? submission?.name : undefined}
                currentValue={submission?.mark}
                title="Override rejected submission"
                label="Mark"
                min={0}
                max={100}
                deciderLabel="lecturer"
                busy={busy}
                onHide={() => setShowOverrideModal(false)}
                onSubmit={handleOverride}
            />
        </>
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