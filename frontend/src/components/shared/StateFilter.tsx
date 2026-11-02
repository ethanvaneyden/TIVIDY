import { Form } from 'react-bootstrap';
import type { WorkflowItemState } from '../../types/types';

const OPTIONS: Array<{ value: WorkflowItemState | ''; label: string }> = [
    { value: '', label: 'All items' },
    { value: 'available', label: 'Available' },
    { value: 'assigned', label: 'Assigned' },
    { value: 'inprogress', label: 'In progress' },
    { value: 'completed', label: 'Completed' },
    { value: 'rejected', label: 'Rejected' },
];

interface StateFilterProps {
    value: WorkflowItemState | '';
    onChange: (value: WorkflowItemState | '') => void;
}

function StateFilter({ value, onChange }: StateFilterProps) {
    return (
        <Form.Group controlId="state-filter" className="state-filter">
            <Form.Label className="workflow-section-label mb-1">FILTER BY STATE</Form.Label>
            <Form.Select
                value={value}
                onChange={(event) => onChange(event.target.value as WorkflowItemState | '')}
            >
                {OPTIONS.map((option) => (
                    <option key={option.value || 'all'} value={option.value}>
                        {option.label}
                    </option>
                ))}
            </Form.Select>
        </Form.Group>
    );
}

export default StateFilter;