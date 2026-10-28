import { useState, type SubmitEvent } from 'react';
import { Button, Form, Modal } from 'react-bootstrap';

interface AddAssignmentModalProps {
    show: boolean;
    onHide: () => void;
}

function AddAssignmentModal({ show, onHide }: AddAssignmentModalProps) {
    const [name, setName] = useState('');

    function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
    }

    return (
        <Modal show={show} onHide={onHide} centered>
            <Form onSubmit={handleSubmit}>
                <Modal.Header closeButton><Modal.Title>Add assignment or test</Modal.Title></Modal.Header>
                <Modal.Body>
                    <Form.Label htmlFor="assignment-name">Assignment or test name</Form.Label>
                    <Form.Control id="assignment-name" value={name} onChange={(event) => setName(event.target.value)} maxLength={80} required />
                    <p className="action-placeholder-note">This form is not connected yet. Nothing will be added.</p>
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide}>Cancel</Button>
                    <Button type="submit" className="workflow-primary-button">Add assignment / test</Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default AddAssignmentModal;
