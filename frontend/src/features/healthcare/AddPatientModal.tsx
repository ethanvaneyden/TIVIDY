import { useState, type SubmitEvent } from 'react';
import { Button, Col, Form, Modal, Row } from 'react-bootstrap';

// interface AddPatientModalProps {
//     show: boolean;
//     onHide: () => void;
// }

interface AddPatientModalProps {
    show: boolean;
    onHide: () => void;
    onSubmit: (name: string, age: number) => Promise<boolean>;
}

// function AddPatientModal({ show, onHide }: AddPatientModalProps) {
//     const [name, setName] = useState('');
//     const [age, setAge] = useState('');

//     function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
//         event.preventDefault();
//     }

//     return (
//         <Modal show={show} onHide={onHide} centered>
//             <Form onSubmit={handleSubmit}>
//                 <Modal.Header closeButton><Modal.Title>Add patient</Modal.Title></Modal.Header>
//                 <Modal.Body>
//                     <Row className="g-3">
//                         <Col sm={8}>
//                             <Form.Label htmlFor="patient-name">Patient name</Form.Label>
//                             <Form.Control id="patient-name" value={name} onChange={(event) => setName(event.target.value)} maxLength={80} required />
//                         </Col>
//                         <Col sm={4}>
//                             <Form.Label htmlFor="patient-age">Age</Form.Label>
//                             <Form.Control id="patient-age" type="number" min={0} step={1} value={age} onChange={(event) => setAge(event.target.value)} required />
//                         </Col>
//                     </Row>
//                     <p className="action-placeholder-note">This form is not connected yet. Nothing will be added.</p>
//                 </Modal.Body>
//                 <Modal.Footer>
//                     <Button variant="outline-secondary" onClick={onHide}>Cancel</Button>
//                     <Button type="submit" className="healthcare-primary-button">Add patient</Button>
//                 </Modal.Footer>
//             </Form>
//         </Modal>
//     );
// }

function AddPatientModal({ show, onHide, onSubmit }: AddPatientModalProps) {
    const [name, setName] = useState('');
    const [age, setAge] = useState('');
    const [busy, setBusy] = useState(false);

    async function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
        const numericAge = Number(age);
        if (!Number.isFinite(numericAge) || numericAge < 0) {
            return;
        }
        setBusy(true);
        const ok = await onSubmit(name.trim(), numericAge);
        setBusy(false);
        if (ok) {
            setName('');
            setAge('');
            onHide();
        }
    }

    return (
        <Modal show={show} onHide={onHide} centered>
            <Form onSubmit={handleSubmit}>
                <Modal.Header closeButton><Modal.Title>Add patient</Modal.Title></Modal.Header>
                <Modal.Body>
                    <Row className="g-3">
                        <Col sm={8}>
                            <Form.Label htmlFor="patient-name">Patient name</Form.Label>
                            <Form.Control
                                id="patient-name"
                                value={name}
                                onChange={(event) => setName(event.target.value)}
                                maxLength={80}
                                required
                            />
                        </Col>
                        <Col sm={4}>
                            <Form.Label htmlFor="patient-age">Age</Form.Label>
                            <Form.Control
                                id="patient-age"
                                type="number"
                                min={0}
                                step={1}
                                value={age}
                                onChange={(event) => setAge(event.target.value)}
                                required
                            />
                        </Col>
                    </Row>
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide} disabled={busy}>
                        Cancel
                    </Button>
                    <Button
                        type="submit"
                        className="healthcare-primary-button"
                        disabled={busy || !name.trim() || !age}
                    >
                        {busy ? 'Adding…' : 'Add patient'}
                    </Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default AddPatientModal;