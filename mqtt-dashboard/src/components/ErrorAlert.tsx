type Props = { error: string };
const ErrorAlert = (props: Props) => {
  if (props.error.length === 0) return <span></span>;
  else return <h1 className="tx-3xl font-bold">{props.error}</h1>;
};

export default ErrorAlert;
