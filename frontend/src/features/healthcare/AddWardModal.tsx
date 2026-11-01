import { useState, type SubmitEvent } from 'react';
import { Button, Form, Modal } from 'react-bootstrap';

interface AddWardModalProps {
    show: boolean;
    busy: boolean;
    onHide: () => void;
    onSubmit: (name: string) => Promise<boolean>;
}

function AddWardModal({ show, busy, onHide, onSubmit }: AddWardModalProps) {
    const [name, setName] = useState('');
    const trimmed = name.trim();

    async function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
        if (!trimmed) return;
        if (await onSubmit(trimmed)) {
            setName('');
            onHide();
        }
    }

    return (
        <Modal show={show} onHide={onHide} centered>
            <Form onSubmit={handleSubmit}>
                <Modal.Header closeButton><Modal.Title>Add ward</Modal.Title></Modal.Header>
                <Modal.Body>
                    <Form.Label htmlFor="ward-name">Ward name</Form.Label>
                    <Form.Control id="ward-name" value={name} onChange={(event) => setName(event.target.value)} maxLength={80} required autoFocus />
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide} disabled={busy}>Cancel</Button>
                    <Button type="submit" className="healthcare-primary-button" disabled={busy || !trimmed}>
                        {busy ? 'Adding...' : 'Add ward'}
                    </Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default AddWardModal;
