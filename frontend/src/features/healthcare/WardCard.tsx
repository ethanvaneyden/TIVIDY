import { Button } from 'react-bootstrap';
import type { Treatment, TreatmentContainer } from '../../types/types';
import TreatmentRow from './TreatmentRow';

// interface WardCardProps {
//     ward: TreatmentContainer;
//     onViewTreatment: (treatment: Treatment) => void;
//     onAddPatient: () => void;
// }

interface WardCardProps {
    ward: TreatmentContainer;
    onViewTreatment: (treatment: Treatment) => void;
    onAddPatient: () => void;
    onApprove: () => void;
}

// function WardCard({ ward, onViewTreatment, onAddPatient }: WardCardProps) {
//     return (
//         <section className="admissions-panel">
//             <div className="admissions-panel-header">
//                 <div>
//                     <span className="workflow-section-label">WARD</span>
//                     <h3>{ward.name}</h3>
//                 </div>
//                 <div className="ward-header-actions">
//                     <span className={`phase-pill phase-${ward.phase.toLowerCase()}`}>{ward.phase}</span>
//                     <Button variant="outline-primary" size="sm" onClick={onAddPatient}>
//                         + Add patient
//                     </Button>
//                 </div>
//             </div>
//             <div className="treatment-list">
//                 {ward.items.length > 0 ? ward.items.map((treatment) => (
//                     <TreatmentRow
//                         key={treatment.id}
//                         treatment={treatment}
//                         onView={() => onViewTreatment(treatment)}
//                     />
//                 )) : (
//                     <div className="workflow-empty-state">
//                         <h4>No patients in this ward</h4>
//                         <p>No treatment items are available.</p>
//                     </div>
//                 )}
//             </div>
//         </section>
//     );
// }

function WardCard({ ward, onViewTreatment, onAddPatient, onApprove }: WardCardProps) {
    const allCompleted = ward.items.length > 0 &&
        ward.items.every((item) => item.state === 'completed' || item.state === 'rejected');
    const canApprove = ward.phase === 'Review' || (ward.phase === 'Active' && allCompleted);

    return (
        <section className="admissions-panel">
            <div className="admissions-panel-header">
                <div>
                    <span className="workflow-section-label">WARD</span>
                    <h3>{ward.name}</h3>
                </div>
                <div className="ward-header-actions">
                    <span className={`phase-pill phase-${ward.phase.toLowerCase()}`}>{ward.phase}</span>
                    {canApprove && (
                        <Button variant="primary" size="sm" onClick={onApprove}>
                            Approve all
                        </Button>
                    )}
                    <Button variant="outline-primary" size="sm" onClick={onAddPatient}>
                        + Add patient
                    </Button>
                </div>
            </div>
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