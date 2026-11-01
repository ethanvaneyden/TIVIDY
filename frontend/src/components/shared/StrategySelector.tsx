import { Form } from 'react-bootstrap';

const STRATEGIES = ['Workload', 'RoundRobin', 'Random'] as const;

interface StrategySelectorProps {
    value: string;
    disabled?: boolean;
    onChange: (name: string) => void;
}

function StrategySelector({ value, disabled, onChange }: StrategySelectorProps) {
    return (
        <Form.Group controlId="strategy-selector" className="strategy-selector">
            <Form.Label className="workflow-section-label mb-1">ASSIGNMENT STRATEGY</Form.Label>
            <Form.Select
                value={value}
                disabled={disabled}
                onChange={(event) => onChange(event.target.value)}
            >
                {STRATEGIES.map((name) => (
                    <option key={name} value={name}>{name}</option>
                ))}
            </Form.Select>
        </Form.Group>
    );
}

export default StrategySelector;