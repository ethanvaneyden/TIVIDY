import { useState } from 'react';
import { Button, Alert, Spinner } from 'react-bootstrap';
import AddPatientModal from './AddPatientModal';
import AddWardModal from './AddWardModal';
import TreatmentDetailsModal from './TreatmentDetailsModal';
import WardCard from './WardCard';
import HistoryPanel from '../../components/shared/HistoryPanel';
import FilteredItemList from '../../components/shared/FilteredItemList';
import StateFilter from '../../components/shared/StateFilter';
// import { getMockHospital } from '../../data/mockData';
import { useHospital } from '../../hooks/useHospital';
import { useFilteredItems } from '../../hooks/useFilteredItems';
import type { Treatment, TreatmentContainer, WorkflowItemState } from '../../types/types';
import '../../styles/ScenarioWorkflowContent.css';
import '../../styles/ContainerCard.css';
import StrategySelector from '../../components/shared/StrategySelector';
import type { FilteredItem } from '../../api/workflow';

interface HealthcareWorkflowProps {
    instanceId: string;
}

function toTreatment(item: FilteredItem): Treatment {
    return {
        id: item.id,
        name: item.name,
        age: item.age ?? 0,
        diagnosis: item.diagnosis ?? '',
        state: normalizeState(item.state),
        assignee: item.assignee ?? '',
        reviewer: item.reviewer ?? '',
        severity: item.result,
        bed: item.bed,
        discharged: item.discharged,
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

// function HealthcareWorkflow({ instanceId }: HealthcareWorkflowProps) {
//     const hospital = getMockHospital(instanceId);
//     const [selectedTreatment, setSelectedTreatment] = useState<Treatment>();
//     const [showAddWardModal, setShowAddWardModal] = useState(false);
//     const [showAddPatientModal, setShowAddPatientModal] = useState(false);

//     return (
//         <div className="workflow-content healthcare-content">
//             <header className="workflow-instance-heading">
//                 <div>
//                     <p className="workflow-instance-kicker">HOSPITAL</p>
//                     <h2>{hospital.name}</h2>
//                     <p>Wards and their treatment items.</p>
//                 </div>
//                 <Button className="healthcare-primary-button" onClick={() => setShowAddWardModal(true)}>
//                     + Add ward
//                 </Button>
//             </header>

//             <section className="ward-container-list" aria-label="Hospital wards">
//                 {hospital.wards.map((ward: TreatmentContainer) => (
//                     <WardCard
//                         key={ward.id}
//                         ward={ward}
//                         onViewTreatment={setSelectedTreatment}
//                         onAddPatient={() => setShowAddPatientModal(true)}
//                     />
//                 ))}
//                 {hospital.wards.length === 0 && (
//                     <div className="workflow-empty-state">
//                         <h4>No wards yet</h4>
//                         <p>No ward containers are available.</p>
//                     </div>
//                 )}
//             </section>

//             <TreatmentDetailsModal
//                 treatment={selectedTreatment}
//                 onHide={() => setSelectedTreatment(undefined)}
//             />
//             <AddWardModal
//                 show={showAddWardModal}
//                 onHide={() => setShowAddWardModal(false)}
//             />
//             <AddPatientModal
//                 show={showAddPatientModal}
//                 onHide={() => setShowAddPatientModal(false)}
//             />
//         </div>
//     );
// }

function HealthcareWorkflow({ instanceId }: HealthcareWorkflowProps) {
    const {
        hospital,
        history,
        loading,
        busy,
        loadError,
        actionError,
        clearActionError,
        strategy,
        changeStrategy,
        addWard,
        addPatient,
        startTreatment,
        completeTreatment,
        approveWard,
        overrideTreatment,
    } = useHospital(instanceId);

    const [selectedTreatment, setSelectedTreatment] = useState<Treatment>();
    const [activeWardId, setActiveWardId] = useState<string>();
    const [showAddWardModal, setShowAddWardModal] = useState(false);
    const [showAddPatientModal, setShowAddPatientModal] = useState(false);
    const [stateFilter, setStateFilter] = useState<WorkflowItemState | ''>('');

    const backendState = stateFilter === '' ? '' : capitalize(stateFilter);
    const { items: filteredItems, loading: filterLoading, error: filterError } =
        useFilteredItems('healthcare', instanceId, backendState);

    if (loading) {
        return <div className="text-center py-5"><Spinner animation="border" /></div>;
    }
    if (loadError) {
        return <Alert variant="danger">{loadError}</Alert>;
    }
    if (!hospital) {
        return <Alert variant="warning">No hospital instance data.</Alert>;
    }

    return (
        <div className="workflow-content healthcare-content">
            <header className="workflow-instance-heading">
                <div>
                    <p className="workflow-instance-kicker">HOSPITAL</p>
                    <h2>{hospital.name}</h2>
                    <p>Wards and their treatment items.</p>
                </div>
                <Button
                    className="healthcare-primary-button"
                    onClick={() => setShowAddWardModal(true)}
                    disabled={busy}
                >
                    + Add ward
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
                <section className="workflow-filtered" aria-label="Filtered patients">
                    <div className="chooser-list-heading">
                        <h2>Matching patients</h2>
                        <span>
                            {filterLoading ? 'Loading…' : `${filteredItems.length} found (from backend)`}
                        </span>
                    </div>
                    {filterError ? (
                        <Alert variant="danger">{filterError}</Alert>
                    ) : (
                        <FilteredItemList
                            items={filteredItems.map(toTreatment)}
                            emptyMessage="No patients match that state."
                            onSelect={(item) => setSelectedTreatment(item as Treatment)}
                        />
                    )}
                </section>
            ) : (
                <section className="ward-container-list" aria-label="Hospital wards">
                    {hospital.wards.map((ward: TreatmentContainer) => (
                        <WardCard
                            key={ward.id}
                            ward={ward}
                            onViewTreatment={setSelectedTreatment}
                            onAddPatient={() => {
                                setActiveWardId(ward.id);
                                setShowAddPatientModal(true);
                            }}
                            onApprove={() => approveWard(ward.id)}
                        />
                    ))}
                    {hospital.wards.length === 0 && (
                        <div className="workflow-empty-state">
                            <h4>No wards yet</h4>
                            <p>Create your first ward to get started.</p>
                        </div>
                    )}
                </section>
            )}

            <TreatmentDetailsModal
                treatment={selectedTreatment}
                onHide={() => setSelectedTreatment(undefined)}
                busy={busy}
                onStart={startTreatment}
                onComplete={completeTreatment}
                onOverride={overrideTreatment}
            />

            <HistoryPanel entries={history} />

            <AddWardModal
                show={showAddWardModal}
                onHide={() => setShowAddWardModal(false)}
                onSubmit={addWard}
            />

            <AddPatientModal
                show={showAddPatientModal}
                onHide={() => setShowAddPatientModal(false)}
                onSubmit={async (name: string, age: number) => {
                    if (!activeWardId) return false;
                    return addPatient(activeWardId, { name, age, diagnosis: '' });
                }}
            />
        </div>
    );
}

export default HealthcareWorkflow;