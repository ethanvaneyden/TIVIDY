import "./EducationWorkflow.css"
import { Container } from "react-bootstrap"
import { useParams } from "react-router";

function EducationWorkflow() {

    const { instanceid } = useParams(); 
    return(
        <Container>
            <h1>Welcome to education workflow</h1>
            <h1>{instanceid}</h1>
        </Container>
    )
}

export default EducationWorkflow

