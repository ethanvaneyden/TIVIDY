import { Button } from 'react-bootstrap';
import type { Treatment, TreatmentContainer } from '../../types/types';
import TreatmentRow from './TreatmentRow';

interface WardCardProps {
    ward: TreatmentContainer;
    busy: boolean;
    onViewTreatment: (treatment: Treatment) => void;
    onAddPatient: () => void;
    onOpenWard: () => void;
    onApprove: () => void;
    onStartTreatment: (treatment: Treatment) => void;
    onCompleteTreatment: (treatment: Treatment) => void;
    onResolveTreatment: (treatment: Treatment) => void;
}

function WardCard({
    ward,
    busy,
    onViewTreatment,
    onAddPatient,
    onOpenWard,
    onApprove,
    onStartTreatment,
    onCompleteTreatment,
    onResolveTreatment,
}: WardCardProps) {
    const inIntake = ward.phase === 'Intake';

    return (
        <section className="admissions-panel">
            <div className="admissions-panel-header">
                <div>
                    <span className="workflow-section-label">WARD</span>
                    <h3>{ward.name}</h3>
                </div>
                <div className="ward-header-actions">
                    <span className={`phase-pill phase-${ward.phase.toLowerCase()}`}>{ward.phase}</span>
                    {inIntake && (
                        <>
                            <Button variant="outline-primary" size="sm" onClick={onAddPatient} disabled={busy}>
                                + Add patient
                            </Button>
                            <Button
                                variant="primary"
                                size="sm"
                                onClick={onOpenWard}
                                disabled={busy || ward.items.length === 0}
                                title={ward.items.length === 0 ? 'Add at least one patient first' : undefined}
                            >
                                Open ward
                            </Button>
                        </>
                    )}
                    {ward.phase === 'Review' && (
                        <Button variant="success" size="sm" onClick={onApprove} disabled={busy}>
                            Approve batch
                        </Button>
                    )}
                </div>
            </div>
            <div className="treatment-list">
                {ward.items.length > 0 ? ward.items.map((treatment) => (
                    <TreatmentRow
                        key={treatment.id}
                        treatment={treatment}
                        phase={ward.phase}
                        busy={busy}
                        onView={() => onViewTreatment(treatment)}
                        onStart={() => onStartTreatment(treatment)}
                        onComplete={() => onCompleteTreatment(treatment)}
                        onResolve={() => onResolveTreatment(treatment)}
                    />
                )) : (
                    <div className="workflow-empty-state">
                        <h4>No patients in this ward</h4>
                        <p>Add a patient to start the admission.</p>
                    </div>
                )}
            </div>
        </section>
    );
}

export default WardCard;
