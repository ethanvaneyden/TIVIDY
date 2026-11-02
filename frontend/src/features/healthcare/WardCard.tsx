import { Button } from 'react-bootstrap';
import type { Treatment, TreatmentContainer } from '../../types/types';
import TreatmentRow from './TreatmentRow';

interface WardCardProps {
    ward: TreatmentContainer;
    onViewTreatment: (treatment: Treatment) => void;
    onAddPatient: () => void;
    onApprove: () => void;
    onOpen: () => void;
    busy?: boolean;
}

function WardCard({ ward, onViewTreatment, onAddPatient, onApprove, onOpen, busy = false }: WardCardProps) {
    const canApprove = ward.phase === 'Review';
    const canOpen = ward.phase === 'Intake' && ward.items.length > 0;
    const canAdd = ward.phase === 'Intake';

    return (
        <section className="admissions-panel">
            <div className="admissions-panel-header">
                <div>
                    <span className="workflow-section-label">WARD</span>
                    <h3>{ward.name}</h3>
                </div>
                <div className="ward-header-actions">
                    <span className={`phase-pill phase-${ward.phase.toLowerCase()}`}>{ward.phase}</span>
                    {canOpen && (
                        <Button variant="success" size="sm" onClick={onOpen} disabled={busy}>
                            Open ward
                        </Button>
                    )}
                    {canApprove && (
                        <Button variant="primary" size="sm" onClick={onApprove} disabled={busy}>
                            Approve all
                        </Button>
                    )}
                    <Button variant="outline-primary" size="sm" onClick={onAddPatient} disabled={!canAdd || busy}>
                        + Add patient
                    </Button>
                </div>
            </div>
            {ward.phase === 'Intake' && ward.items.length > 0 && (
                <p className="container-hint px-4 pt-3 mb-0">
                    Patients can be admitted until the ward is opened. Opening assigns a doctor to every patient.
                </p>
            )}
            <div className="treatment-list">
                {ward.items.length > 0 ? ward.items.map((treatment) => (
                    <TreatmentRow
                        key={treatment.id}
                        treatment={treatment}
                        onView={() => onViewTreatment(treatment)}
                    />
                )) : (
                    <div className="workflow-empty-state">
                        <h4>No patients in this ward</h4>
                        <p>No treatment items are available.</p>
                    </div>
                )}
            </div>
        </section>
    );
}

export default WardCard;