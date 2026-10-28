import { useState } from 'react';
import { Button } from 'react-bootstrap';
import AddAssignmentModal from './AddAssignmentModal';
import AddSubmissionModal from './AddSubmissionModal';
import ContainerCard from './ContainerCard';
import SubmissionDetailsModal from './SubmissionDetailsModal';
import { getMockEducationInstance } from '../../data/mockData';
import type { Assignment, Submission } from '../../types/types';
import '../../styles/ScenarioWorkflowContent.css';

interface EducationWorkflowProps {
    instanceId: string;
}

function EducationWorkflow({ instanceId }: EducationWorkflowProps) {
    const instance = getMockEducationInstance(instanceId);
    const [selectedSubmission, setSelectedSubmission] = useState<Submission>();
    const [showAddAssignmentModal, setShowAddAssignmentModal] = useState(false);
    const [showAddSubmissionModal, setShowAddSubmissionModal] = useState(false);

    return (
        <div className="workflow-content">
            <header className="workflow-instance-heading">
                <div>
                    <p className="workflow-instance-kicker">INSTANCE</p>
                    <h2>{instance.name}</h2>
                    <p>Assignments and tests within this instance.</p>
                </div>
                <Button className="workflow-primary-button" onClick={() => setShowAddAssignmentModal(true)}>
                    + Add assignment / test
                </Button>
            </header>

            <section className="assignment-container-list" aria-label="Assignments and tests">
                {instance.assignments.map((assignment: Assignment) => (
                    <ContainerCard
                        key={assignment.id}
                        assignment={assignment}
                        onViewSubmission={setSelectedSubmission}
                        onAddSubmission={() => setShowAddSubmissionModal(true)}
                    />
                ))}
                {instance.assignments.length === 0 && (
                    <div className="workflow-empty-state">
                        <h4>No assignments or tests yet</h4>
                        <p>No assignment or test containers are available.</p>
                    </div>
                )}
            </section>

            <SubmissionDetailsModal
                submission={selectedSubmission}
                onHide={() => setSelectedSubmission(undefined)}
            />
            <AddAssignmentModal
                show={showAddAssignmentModal}
                onHide={() => setShowAddAssignmentModal(false)}
            />
            <AddSubmissionModal
                show={showAddSubmissionModal}
                onHide={() => setShowAddSubmissionModal(false)}
            />
        </div>
    );
}

export default EducationWorkflow;
