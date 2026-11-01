import { Button } from 'react-bootstrap';
import type { Treatment, WorkflowPhase } from '../../types/types';

interface TreatmentRowProps {
    treatment: Treatment;
    phase: WorkflowPhase;
    busy: boolean;
    onView: () => void;
    onStart: () => void;
    onComplete: () => void;
    onResolve: () => void;
}

const stateLabels: Record<Treatment['state'], string> = {
    available: 'Available',
    assigned: 'Assigned',
    inprogress: 'In progress',
    completed: 'Completed',
    rejected: 'Rejected',
};

function TreatmentRow({ treatment, phase, busy, onView, onStart, onComplete, onResolve }: TreatmentRowProps) {
    const canStart = phase === 'Active' && treatment.state === 'assigned';
    const canComplete = phase === 'Active' && treatment.state === 'inprogress';
    const canResolve = phase === 'Review' && treatment.state === 'rejected';

    return (
        <article className={`treatment-row treatment-state-${treatment.state}`}>
            <div className="treatment-patient">
                <span className="treatment-avatar" aria-hidden="true">{getInitials(treatment.name)}</span>
                <div>
                    <h4>{treatment.name}</h4>
                    <p>{treatment.age !== undefined ? `${treatment.age} years · ` : ''}{treatment.id}</p>
                </div>
            </div>
            <div className="treatment-diagnosis">
                <span className="treatment-label">DIAGNOSIS</span>
                <strong>{treatment.diagnosis || 'Not recorded'}</strong>
                {treatment.severity !== undefined && <span>Severity {treatment.severity}/10</span>}
            </div>
            <div className="treatment-assignee">
                <span className="treatment-label">DOCTOR</span>
                <strong>{treatment.assignee || 'Not assigned'}</strong>
                {treatment.reviewer && <span className="treatment-reviewer">Reviewed by {treatment.reviewer}</span>}
            </div>
            <span className={`treatment-status treatment-status-${treatment.state}`}>
                <span />
                {stateLabels[treatment.state]}
            </span>
            <div className="treatment-actions row-actions">
                {canStart && (
                    <Button size="sm" variant="primary" onClick={onStart} disabled={busy}>Start</Button>
                )}
                {canComplete && (
                    <Button size="sm" variant="success" onClick={onComplete} disabled={busy}>Complete</Button>
                )}
                {canResolve && (
                    <Button size="sm" variant="warning" onClick={onResolve} disabled={busy}>Resolve</Button>
                )}
                <Button size="sm" variant="outline-secondary" onClick={onView}>Details</Button>
            </div>
        </article>
    );
}

function getInitials(name: string): string {
    return name.split(' ').map((part) => part[0] ?? '').slice(0, 2).join('').toUpperCase();
}

export default TreatmentRow;
