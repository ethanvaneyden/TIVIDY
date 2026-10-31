import { useState } from 'react';
import { Alert, Button } from 'react-bootstrap';
import AddAssignmentModal from './AddAssignmentModal';
import AddSubmissionModal from './AddSubmissionModal';
import ContainerCard from './ContainerCard';
import SubmissionDetailsModal from './SubmissionDetailsModal';
import CompleteItemModal from '../../components/modals/CompleteItemModal';
import OverrideItemModal from '../../components/modals/OverrideItemModal';
import HistoryPanel from '../../components/shared/HistoryPanel';
import { LoadFailure, Loading } from '../../components/shared/LoadState';
import { useEducation } from '../../hooks/useEducation';
import type { Submission } from '../../types/types';
import '../../styles/ScenarioWorkflowContent.css';
import '../../styles/Shared.css';

interface EducationWorkflowProps {
    instanceId: string;
}

const MARK_HINT =
    'Below 40 cannot be approved and is rejected at review. 40-49 is approved by a moderator, ' +
    '50-70 by a TA and 71-100 by the lecturer.';

function EducationWorkflow({ instanceId }: EducationWorkflowProps) {
    const edu = useEducation(instanceId);
    const [selectedSubmission, setSelectedSubmission] = useState<Submission>();
    const [completing, setCompleting] = useState<Submission>();
    const [overriding, setOverriding] = useState<Submission>();
    const [showAddAssignmentModal, setShowAddAssignmentModal] = useState(false);
    const [addSubmissionAssignmentId, setAddSubmissionAssignmentId] = useState<string>();

    if (edu.loading && !edu.education) {
        return (
            <div className="workflow-content">
                <Loading label="Loading marking workspace..." />
            </div>
        );
    }

    if (!edu.education) {
        return (
            <div className="workflow-content">
                <LoadFailure
                    title="Could not load this instance"
                    message={edu.loadError}
                    onRetry={() => void edu.refresh()}
                />
            </div>
        );
    }

    const instance = edu.education;
    const addSubmissionAssignment = instance.assignments.find(
        (assignment) => assignment.id === addSubmissionAssignmentId,
    );

    return (
        <div className="workflow-content">
            <header className="workflow-instance-heading">
                <div>
                    <p className="workflow-instance-kicker">INSTANCE</p>
                    <h2>{instance.name}</h2>
                    <p>Assignments and tests within this instance.</p>
                </div>
                <div className="workflow-header-buttons">
                    <Button variant="outline-secondary" onClick={() => void edu.refresh()} disabled={edu.busy || edu.loading}>
                        Refresh
                    </Button>
                    <Button className="workflow-primary-button" onClick={() => setShowAddAssignmentModal(true)} disabled={edu.busy}>
                        + Add assignment / test
                    </Button>
                </div>
            </header>

            {edu.actionError && (
                <Alert variant="danger" dismissible onClose={edu.clearActionError}>{edu.actionError}</Alert>
            )}

            <section className="assignment-container-list" aria-label="Assignments and tests">
                {instance.assignments.map((assignment) => (
                    <ContainerCard
                        key={assignment.id}
                        assignment={assignment}
                        busy={edu.busy}
                        onViewSubmission={setSelectedSubmission}
                        onAddSubmission={() => setAddSubmissionAssignmentId(assignment.id)}
                        onOpen={() => void edu.openAssignment(assignment.id)}
                        onApprove={() => void edu.approveAssignment(assignment.id)}
                        onStartSubmission={(submission) => void edu.startSubmission(submission.id)}
                        onCompleteSubmission={setCompleting}
                        onResolveSubmission={setOverriding}
                    />
                ))}
                {instance.assignments.length === 0 && (
                    <div className="workflow-empty-state">
                        <h4>No assignments or tests yet</h4>
                        <p>Add an assignment or test to start collecting submissions.</p>
                    </div>
                )}
            </section>

            <HistoryPanel entries={edu.history} />

            <SubmissionDetailsModal
                submission={selectedSubmission}
                onHide={() => setSelectedSubmission(undefined)}
            />
            <AddAssignmentModal
                show={showAddAssignmentModal}
                busy={edu.busy}
                onHide={() => setShowAddAssignmentModal(false)}
                onSubmit={edu.addAssignment}
            />
            <AddSubmissionModal
                show={Boolean(addSubmissionAssignment)}
                busy={edu.busy}
                assignmentName={addSubmissionAssignment?.name}
                onHide={() => setAddSubmissionAssignmentId(undefined)}
                onSubmit={(submission) => edu.addSubmission(addSubmissionAssignmentId ?? '', submission)}
            />
            <CompleteItemModal
                key={completing?.id ?? 'complete-none'}
                itemName={completing?.name}
                title="Record mark"
                label="Mark"
                min={0}
                max={100}
                hint={MARK_HINT}
                busy={edu.busy}
                onHide={() => setCompleting(undefined)}
                onSubmit={(mark) => edu.completeSubmission(completing?.id ?? '', mark)}
            />
            <OverrideItemModal
                key={overriding?.id ?? 'override-none'}
                itemName={overriding?.name}
                currentValue={overriding?.mark}
                title="Resolve rejected submission"
                label="New mark"
                min={0}
                max={100}
                deciderLabel="lecturer"
                busy={edu.busy}
                onHide={() => setOverriding(undefined)}
                onSubmit={(mark, accept) => edu.overrideSubmission(overriding?.id ?? '', mark, accept)}
            />
        </div>
    );
}

export default EducationWorkflow;
