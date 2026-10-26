import './WorkflowCard.css';
import { useNavigate } from 'react-router';

interface WorkflowCardProps {
    title: string;
    description: string;
    image: string;
    accent: 'blue' | 'violet';
    features: string[];
    link: string;
}

function WorkflowCard({
    title,
    description,
    image,
    accent,
    features,
    link,
}: WorkflowCardProps) {
    const navigate = useNavigate();
    return (
        <div className={`WorkflowCard accent-${accent} d-flex flex-column h-100`}>
            <div className="CardImage">
                <img
                    src={image}
                    alt={title}
                    className="w-100"
                />
            </div>

            <div className="CardBody d-flex flex-column flex-grow-1 gap-3 p-4">
                <h2 className="CardHeader mb-0">
                    {title}
                </h2>

                <p className="CardDescription mb-0">
                    {description}
                </p>

                <ul className="list-unstyled d-flex flex-column gap-3 mb-0">
                    {features.map((feature) => (
                        <li key={feature} className="CardFeature">
                            {feature}
                        </li>
                    ))}
                </ul>
            </div>

            <div className="CardFooter p-4 pt-0 mt-auto"
            onClick={() => navigate(link)}>
                <button className="CardButton w-100">
                    Start workflow
                </button>
            </div>
        </div>
    );
}

export default WorkflowCard;