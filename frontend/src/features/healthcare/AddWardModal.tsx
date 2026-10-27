import { useState, type SubmitEvent } from 'react';
import { Button, Form, Modal } from 'react-bootstrap';

interface AddWardModalProps {
    show: boolean;
    onHide: () => void;
}

function AddWardModal({ show, onHide }: AddWardModalProps) {
    const [name, setName] = useState('');

    function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
    }

    return (
        <Modal show={show} onHide={onHide} centered>
            <Form onSubmit={handleSubmit}>
                <Modal.Header closeButton><Modal.Title>Add ward</Modal.Title></Modal.Header>
                <Modal.Body>
                    <Form.Label htmlFor="ward-name">Ward name</Form.Label>
                    <Form.Control id="ward-name" value={name} onChange={(event) => setName(event.target.value)} maxLength={80} required />
                    <p className="action-placeholder-note">This form is not connected yet. Nothing will be added.</p>
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide}>Cancel</Button>
                    <Button type="submit" className="healthcare-primary-button">Add ward</Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default AddWardModal;
