import { useState, type SubmitEvent } from 'react';
import { Button, Form, Modal } from 'react-bootstrap';
import { parseWhole } from '../../utils/parseWhole';

interface OverrideItemModalProps {
    itemName?: string;
    currentValue?: number;
    title: string;
    label: string;
    min: number;
    max: number;
    deciderLabel: string;
    busy: boolean;
    primaryClassName?: string;
    onHide: () => void;
    onSubmit: (value: number, accept: boolean) => Promise<boolean>;
}

function OverrideItemModal({
    itemName,
    currentValue,
    title,
    label,
    min,
    max,
    deciderLabel,
    busy,
    primaryClassName = 'workflow-primary-button',
    onHide,
    onSubmit,
}: OverrideItemModalProps) {
    const [text, setText] = useState('');
    const [accept, setAccept] = useState(false);
    const value = parseWhole(text, min, max);
    const invalid = !accept && text !== '' && value === undefined;
    const canSubmit = accept ? currentValue !== undefined && currentValue >= min && currentValue <= max : value !== undefined;

    async function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
        if (!canSubmit) return;
        if (await onSubmit(value ?? 0, accept)) {
            setText('');
            setAccept(false);
            onHide();
        }
    }

    return (
        <Modal show={itemName !== undefined} onHide={onHide} centered>
            <Form onSubmit={handleSubmit} noValidate>
                <Modal.Header closeButton><Modal.Title>{title}</Modal.Title></Modal.Header>
                <Modal.Body>
                    <p className="mb-3">
                        <strong>{itemName}</strong> was rejected
                        {currentValue !== undefined ? ` with ${currentValue}` : ''}. The {deciderLabel} settles it.
                    </p>
                    <Form.Check
                        id="override-accept"
                        type="checkbox"
                        label="Keep the recorded value"
                        checked={accept}
                        onChange={(event) => setAccept(event.target.checked)}
                        className="mb-3"
                    />
                    <Form.Label htmlFor="override-value">{label} ({min}-{max})</Form.Label>
                    <Form.Control
                        id="override-value"
                        inputMode="numeric"
                        value={text}
                        onChange={(event) => setText(event.target.value)}
                        isInvalid={invalid}
                        disabled={accept}
                    />
                    <Form.Control.Feedback type="invalid">
                        Enter a whole number from {min} to {max}.
                    </Form.Control.Feedback>
                    {accept && !canSubmit && (
                        <p className="modal-hint">The recorded value is outside {min}-{max}, so it cannot be kept.</p>
                    )}
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide} disabled={busy}>Cancel</Button>
                    <Button type="submit" className={primaryClassName} disabled={busy || !canSubmit}>
                        {busy ? 'Saving...' : 'Resolve'}
                    </Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default OverrideItemModal;
