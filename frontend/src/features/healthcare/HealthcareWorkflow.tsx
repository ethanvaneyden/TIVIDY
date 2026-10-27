import { useState } from 'react';
import { Button } from 'react-bootstrap';
import AddPatientModal from './AddPatientModal';
import AddWardModal from './AddWardModal';
import TreatmentDetailsModal from './TreatmentDetailsModal';
import WardCard from './WardCard';
import { getMockHospital } from '../../data/mockData';
import type { Treatment, TreatmentContainer } from '../../types/types';
import '../../styles/ScenarioWorkflowContent.css';
import '../../styles/ContainerCard.css';

interface HealthcareWorkflowProps {
    instanceId: string;
}

function HealthcareWorkflow({ instanceId }: HealthcareWorkflowProps) {
    const hospital = getMockHospital(instanceId);
    const [selectedTreatment, setSelectedTreatment] = useState<Treatment>();
    const [showAddWardModal, setShowAddWardModal] = useState(false);
    const [showAddPatientModal, setShowAddPatientModal] = useState(false);

    return (
        <div className="workflow-content healthcare-content">
            <header className="workflow-instance-heading">
                <div>
                    <p className="workflow-instance-kicker">HOSPITAL</p>
                    <h2>{hospital.name}</h2>
                    <p>Wards and their treatment items.</p>
                </div>
                <Button className="healthcare-primary-button" onClick={() => setShowAddWardModal(true)}>
                    + Add ward
                </Button>
            </header>

            <section className="ward-container-list" aria-label="Hospital wards">
                {hospital.wards.map((ward: TreatmentContainer) => (
                    <WardCard
                        key={ward.id}
                        ward={ward}
                        onViewTreatment={setSelectedTreatment}
                        onAddPatient={() => setShowAddPatientModal(true)}
                    />
                ))}
                {hospital.wards.length === 0 && (
                    <div className="workflow-empty-state">
                        <h4>No wards yet</h4>
                        <p>No ward containers are available.</p>
                    </div>
                )}
            </section>

            <TreatmentDetailsModal
                treatment={selectedTreatment}
                onHide={() => setSelectedTreatment(undefined)}
            />
            <AddWardModal
                show={showAddWardModal}
                onHide={() => setShowAddWardModal(false)}
            />
            <AddPatientModal
                show={showAddPatientModal}
                onHide={() => setShowAddPatientModal(false)}
            />
        </div>
    );
}

export default HealthcareWorkflow;
