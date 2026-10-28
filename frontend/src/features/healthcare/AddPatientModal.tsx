import { useState, type SubmitEvent } from 'react';
import { Button, Col, Form, Modal, Row } from 'react-bootstrap';

interface AddPatientModalProps {
    show: boolean;
    onHide: () => void;
}

function AddPatientModal({ show, onHide }: AddPatientModalProps) {
    const [name, setName] = useState('');
    const [age, setAge] = useState('');

    function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
    }

    return (
        <Modal show={show} onHide={onHide} centered>
            <Form onSubmit={handleSubmit}>
                <Modal.Header closeButton><Modal.Title>Add patient</Modal.Title></Modal.Header>
                <Modal.Body>
                    <Row className="g-3">
                        <Col sm={8}>
                            <Form.Label htmlFor="patient-name">Patient name</Form.Label>
                            <Form.Control id="patient-name" value={name} onChange={(event) => setName(event.target.value)} maxLength={80} required />
                        </Col>
                        <Col sm={4}>
                            <Form.Label htmlFor="patient-age">Age</Form.Label>
                            <Form.Control id="patient-age" type="number" min={0} step={1} value={age} onChange={(event) => setAge(event.target.value)} required />
                        </Col>
                    </Row>
                    <p className="action-placeholder-note">This form is not connected yet. Nothing will be added.</p>
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide}>Cancel</Button>
                    <Button type="submit" className="healthcare-primary-button">Add patient</Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default AddPatientModal;
