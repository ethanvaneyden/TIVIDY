import { useState } from 'react';
import { Alert, Button } from 'react-bootstrap';
import AddPatientModal from './AddPatientModal';
import AddWardModal from './AddWardModal';
import TreatmentDetailsModal from './TreatmentDetailsModal';
import WardCard from './WardCard';
import CompleteItemModal from '../../components/modals/CompleteItemModal';
import OverrideItemModal from '../../components/modals/OverrideItemModal';
import HistoryPanel from '../../components/shared/HistoryPanel';
import { LoadFailure, Loading } from '../../components/shared/LoadState';
import { useHospital } from '../../hooks/useHospital';
import type { Treatment } from '../../types/types';
import '../../styles/ScenarioWorkflowContent.css';
import '../../styles/ContainerCard.css';
import '../../styles/Shared.css';

interface HealthcareWorkflowProps {
    instanceId: string;
}

const SEVERITY_HINT =
    '1-3 is approved by a triage nurse, 4-6 by an admitting doctor and 7-10 by the chief of medicine. ' +
    'A severity of 0 cannot be approved and is rejected at review.';

function HealthcareWorkflow({ instanceId }: HealthcareWorkflowProps) {
    const hc = useHospital(instanceId);
    const [selectedTreatment, setSelectedTreatment] = useState<Treatment>();
    const [completing, setCompleting] = useState<Treatment>();
    const [overriding, setOverriding] = useState<Treatment>();
    const [showAddWardModal, setShowAddWardModal] = useState(false);
    const [addPatientWardId, setAddPatientWardId] = useState<string>();

    if (hc.loading && !hc.hospital) {
        return (
            <div className="workflow-content">
                <Loading label="Loading hospital..." />
            </div>
        );
    }

    if (!hc.hospital) {
        return (
            <div className="workflow-content">
                <LoadFailure
                    title="Could not load this hospital"
                    message={hc.loadError}
                    onRetry={() => void hc.refresh()}
                />
            </div>
        );
    }

    const hospital = hc.hospital;
    const addPatientWard = hospital.wards.find((ward) => ward.id === addPatientWardId);

    return (
        <div className="workflow-content healthcare-content">
            <header className="workflow-instance-heading">
                <div>
                    <p className="workflow-instance-kicker">HOSPITAL</p>
                    <h2>{hospital.name}</h2>
                    <p>Wards and their treatment items.</p>
                </div>
                <div className="workflow-header-buttons">
                    <Button variant="outline-secondary" onClick={() => void hc.refresh()} disabled={hc.busy || hc.loading}>
                        Refresh
                    </Button>
                    <Button className="healthcare-primary-button" onClick={() => setShowAddWardModal(true)} disabled={hc.busy}>
                        + Add ward
                    </Button>
                </div>
            </header>

            {hc.actionError && (
                <Alert variant="danger" dismissible onClose={hc.clearActionError}>{hc.actionError}</Alert>
            )}

            <section className="ward-container-list" aria-label="Hospital wards">
                {hospital.wards.map((ward) => (
                    <WardCard
                        key={ward.id}
                        ward={ward}
                        busy={hc.busy}
                        onViewTreatment={setSelectedTreatment}
                        onAddPatient={() => setAddPatientWardId(ward.id)}
                        onOpenWard={() => void hc.openWard(ward.id)}
                        onApprove={() => void hc.approveWard(ward.id)}
                        onStartTreatment={(treatment) => void hc.startTreatment(treatment.id)}
                        onCompleteTreatment={setCompleting}
                        onResolveTreatment={setOverriding}
                    />
                ))}
                {hospital.wards.length === 0 && (
                    <div className="workflow-empty-state">
                        <h4>No wards yet</h4>
                        <p>Add a ward to start admitting patients.</p>
                    </div>
                )}
            </section>

            <HistoryPanel entries={hc.history} />

            <TreatmentDetailsModal
                treatment={selectedTreatment}
                onHide={() => setSelectedTreatment(undefined)}
            />
            <AddWardModal
                show={showAddWardModal}
                busy={hc.busy}
                onHide={() => setShowAddWardModal(false)}
                onSubmit={hc.addWard}
            />
            <AddPatientModal
                show={Boolean(addPatientWard)}
                busy={hc.busy}
                wardName={addPatientWard?.name}
                onHide={() => setAddPatientWardId(undefined)}
                onSubmit={(patient) => hc.addPatient(addPatientWardId ?? '', patient)}
            />
            <CompleteItemModal
                key={completing?.id ?? 'complete-none'}
                itemName={completing?.name}
                title="Complete treatment"
                label="Severity"
                min={0}
                max={10}
                hint={SEVERITY_HINT}
                busy={hc.busy}
                primaryClassName="healthcare-primary-button"
                onHide={() => setCompleting(undefined)}
                onSubmit={(severity) => hc.completeTreatment(completing?.id ?? '', severity)}
            />
            <OverrideItemModal
                key={overriding?.id ?? 'override-none'}
                itemName={overriding?.name}
                currentValue={overriding?.severity}
                title="Resolve rejected treatment"
                label="New severity"
                min={1}
                max={10}
                deciderLabel="chief of medicine"
                busy={hc.busy}
                primaryClassName="healthcare-primary-button"
                onHide={() => setOverriding(undefined)}
                onSubmit={(severity, accept) => hc.overrideTreatment(overriding?.id ?? '', severity, accept)}
            />
        </div>
    );
}

export default HealthcareWorkflow;
