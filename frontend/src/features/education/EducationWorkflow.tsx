import { useState } from 'react';
import { Button, Alert, Spinner } from 'react-bootstrap';
import AddAssignmentModal from './AddAssignmentModal';
import AddSubmissionModal from './AddSubmissionModal';
import ContainerCard from './ContainerCard';
import SubmissionDetailsModal from './SubmissionDetailsModal';
import HistoryPanel from '../../components/shared/HistoryPanel';
import FilteredItemList from '../../components/shared/FilteredItemList';
import StateFilter from '../../components/shared/StateFilter';
// import { getMockEducationInstance } from '../../data/mockData';
import { useEducation } from '../../hooks/useEducation';
import { useFilteredItems } from '../../hooks/useFilteredItems';
import type { Assignment, Submission, WorkflowItemState } from '../../types/types';
import '../../styles/ScenarioWorkflowContent.css';
import StrategySelector from '../../components/shared/StrategySelector';
import type { FilteredItem } from '../../api/workflow';

interface EducationWorkflowProps {
    instanceId: string;
}

function toSubmission(item: FilteredItem): Submission {
    return {
        id: item.id,
        name: item.name,
        state: normalizeState(item.state),
        assignee: item.assignee ?? '',
        reviewer: item.reviewer ?? '',
        mark: item.result,
    };
}

function normalizeState(name: string): WorkflowItemState {
    switch (name.toLowerCase()) {
        case 'assigned': return 'assigned';
        case 'inprogress': return 'inprogress';
        case 'completed': return 'completed';
        case 'rejected': return 'rejected';
        default: return 'available';
    }
}

function capitalize(s: string): string {
    if (s === 'inprogress') return 'InProgress';
    return s.charAt(0).toUpperCase() + s.slice(1);
}

// function EducationWorkflow({ instanceId }: EducationWorkflowProps) {
//     const instance = getMockEducationInstance(instanceId);
//     const [selectedSubmission, setSelectedSubmission] = useState<Submission>();
//     const [showAddAssignmentModal, setShowAddAssignmentModal] = useState(false);
//     const [showAddSubmissionModal, setShowAddSubmissionModal] = useState(false);

//     return (
//         <div className="workflow-content">
//             <header className="workflow-instance-heading">
//                 <div>
//                     <p className="workflow-instance-kicker">INSTANCE</p>
//                     <h2>{instance.name}</h2>
//                     <p>Assignments and tests within this instance.</p>
//                 </div>
//                 <Button className="workflow-primary-button" onClick={() => setShowAddAssignmentModal(true)}>
//                     + Add assignment / test
//                 </Button>
//             </header>

//             <section className="assignment-container-list" aria-label="Assignments and tests">
//                 {instance.assignments.map((assignment: Assignment) => (
//                     <ContainerCard
//                         key={assignment.id}
//                         assignment={assignment}
//                         onViewSubmission={setSelectedSubmission}
//                         onAddSubmission={() => setShowAddSubmissionModal(true)}
//                     />
//                 ))}
//                 {instance.assignments.length === 0 && (
//                     <div className="workflow-empty-state">
//                         <h4>No assignments or tests yet</h4>
//                         <p>No assignment or test containers are available.</p>
//                     </div>
//                 )}
//             </section>

//             <SubmissionDetailsModal
//                 submission={selectedSubmission}
//                 onHide={() => setSelectedSubmission(undefined)}
//             />
//             <AddAssignmentModal
//                 show={showAddAssignmentModal}
//                 onHide={() => setShowAddAssignmentModal(false)}
//             />
//             <AddSubmissionModal
//                 show={showAddSubmissionModal}
//                 onHide={() => setShowAddSubmissionModal(false)}
//             />
//         </div>
//     );
// }

function EducationWorkflow({ instanceId }: EducationWorkflowProps) {
    const {
        education,
        history,
        loading,
        busy,
        loadError,
        actionError,
        clearActionError,
        strategy,
        changeStrategy,
        addAssignment,
        addSubmission,
        startSubmission,
        completeSubmission,
        approveAssignment,
        overrideSubmission,
    } = useEducation(instanceId);

    const [selectedSubmission, setSelectedSubmission] = useState<Submission>();
    const [activeAssignmentId, setActiveAssignmentId] = useState<string>();
    const [showAddAssignmentModal, setShowAddAssignmentModal] = useState(false);
    const [showAddSubmissionModal, setShowAddSubmissionModal] = useState(false);
    const [stateFilter, setStateFilter] = useState<WorkflowItemState | ''>('');

    const backendState = stateFilter === '' ? '' : capitalize(stateFilter);
    const { items: filteredItems, loading: filterLoading, error: filterError } =
        useFilteredItems('education', instanceId, backendState);

    if (loading) {
        return <div className="text-center py-5"><Spinner animation="border" /></div>;
    }
    if (loadError) {
        return <Alert variant="danger">{loadError}</Alert>;
    }
    if (!education) {
        return <Alert variant="warning">No education instance data.</Alert>;
    }

    return (
        <div className="workflow-content">
            <header className="workflow-instance-heading">
                <div>
                    <p className="workflow-instance-kicker">INSTANCE</p>
                    <h2>{education.name}</h2>
                    <p>Assignments and tests within this instance.</p>
                </div>
                <Button
                    className="workflow-primary-button"
                    onClick={() => setShowAddAssignmentModal(true)}
                    disabled={busy}
                >
                    + Add assignment / test
                </Button>
            </header>

            {actionError && (
                <Alert variant="warning" dismissible onClose={clearActionError}>
                    {actionError}
                </Alert>
            )}

            <div className="workflow-filter-row d-flex justify-content-between align-items-end gap-3 mb-4">
                <StateFilter value={stateFilter} onChange={setStateFilter} />
                <StrategySelector
                    value={strategy}
                    disabled={busy}
                    onChange={(name) => { void changeStrategy(name); }}
                />
            </div>

            {stateFilter !== '' ? (
                <section className="workflow-filtered" aria-label="Filtered submissions">
                    <div className="chooser-list-heading">
                        <h2>Matching submissions</h2>
                        <span>
                            {filterLoading ? 'Loading…' : `${filteredItems.length} found (from backend)`}
                        </span>
                    </div>
                    {filterError ? (
                        <Alert variant="danger">{filterError}</Alert>
                    ) : (
                        <FilteredItemList
                            items={filteredItems.map(toSubmission)}
                            emptyMessage="No submissions match that state."
                            onSelect={(item) => setSelectedSubmission(item as Submission)}
                        />
                    )}
                </section>
            ) : (
                <section className="assignment-container-list" aria-label="Assignments and tests">
                    {education.assignments.map((assignment: Assignment) => (
                        <ContainerCard
                            key={assignment.id}
                            assignment={assignment}
                            onViewSubmission={setSelectedSubmission}
                            onAddSubmission={() => {
                                setActiveAssignmentId(assignment.id);
                                setShowAddSubmissionModal(true);
                            }}
                            onApprove={() => approveAssignment(assignment.id)}
                        />
                    ))}
                    {education.assignments.length === 0 && (
                        <div className="workflow-empty-state">
                            <h4>No assignments or tests yet</h4>
                            <p>Create your first assignment to get started.</p>
                        </div>
                    )}
                </section>
            )}

            <SubmissionDetailsModal
                submission={selectedSubmission}
                onHide={() => setSelectedSubmission(undefined)}
                busy={busy}
                onStart={startSubmission}
                onComplete={completeSubmission}
                onOverride={overrideSubmission}
            />

            <HistoryPanel entries={history} />

            <AddAssignmentModal
                show={showAddAssignmentModal}
                onHide={() => setShowAddAssignmentModal(false)}
                onSubmit={addAssignment}
            />

            <AddSubmissionModal
                show={showAddSubmissionModal}
                onHide={() => setShowAddSubmissionModal(false)}
                onSubmit={async (studentName: string) => {
                    if (!activeAssignmentId) return false;
                    return addSubmission(activeAssignmentId, { studentName });
                }}
            />
        </div>
    );
}

// function findAssignmentId(instance: { assignments: Assignment[] }, submission: Submission | undefined): string | undefined {
//     if (!submission) return undefined;
//     return instance.assignments.find((assignment) =>
//         assignment.items.some((item) => item.id === submission.id)
//     )?.id;
// }

export default EducationWorkflow;
