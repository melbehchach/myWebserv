#include "./headers/UriParser.hpp"

UriParser::UriParser() : HostStateParser(h_start, h_done),
						 _Uri(NULL),
						 _Groops(0),
						 _Colons(0),
						 _Numbers(0),
						 _Lit(false) {}

UriParser::~UriParser() {}

size_t UriParser::Parse(Uri &uri, std::string const &inpt)
{
	_Groops = 0;
	_Colons = 0;
	_Numbers = 0;
	_Lit = false;
	_Uri = &uri;
	return ParseString(inpt);
}

HostState UriParser::PushBuffer(std::string &field, HostState next)
{
	if (!_Buf.empty())
	{
		field = _Buf;
		_Buf.clear();
	}
	_SkipChar = true;
	return next;
}

HostState UriParser::LiteralHandler(size_t index)
{
	switch (_Input[index])
	{
	case 'v':
		return h_IpvF;
	case ':':
		return h_Ipv6;
	default:
		if (IsHexDig(_Input[index]))
			return h_Ipv6;
		else
			return h_invalid;
	}
}

HostState UriParser::RegNameHndler(size_t index)
{
	switch (_Input[index])
	{
	case '\0':
		return PushBuffer(_Uri->_Host, h_done);
	case ':':
		return PushBuffer(_Uri->_Host, h_port);
	case '%':
		return h_regnamepct;
	default:
		if (IsUnreservedSubDelim(_Input[index]))
			return h_regname;
		return h_invalid;
	}
}

HostState UriParser::IpvFHndler(size_t index)
{
	_Groops++;
	switch (_Input[index])
	{
	case ']':
		if (_Groops >= 4)
			return h_literalend;
		break;
	case '.':
		if (IsHexDig(_Buf.back()))
			return h_IpvF;
		break;
	default:
		if (_Buf.back() == 'v' && IsHexDig(_Input[index]))
			return h_IpvF;
		else if (IsUnreservedSubDelim(_Input[index]) || _Input[index] == ':')
			return h_IpvF;
		else
			return h_invalid;
	}
	return h_invalid;
}

// handels IP literals signald by '['
HostState UriParser::LiteralEndHndler(size_t index)
{
	switch (_Input[index])
	{
	case '\0':
		return PushBuffer(_Uri->_Host, h_done);
	case ':':
		return PushBuffer(_Uri->_Host, h_port);
	default:
		return h_invalid;
	}
}

HostState UriParser::Ipv6Hndler(size_t index)
{
	switch (_Input[index])
	{
	case ']':
		if (_Groops >= 1)
			return h_literalend;
		break;
	case ':':
		return HandelIpv6Colone(_Colons, _Numbers, _Groops);
	default:
		if (ValidLastBitsIpv4(_Input, index))
		{
			_Groops = 0;
			_Numbers = 0;
			return h_Ipv4;
		}
		else if (IsHexDig(_Input[index]))
		{
			NormalizeIPv6HexDig(_Buf, _Input[index], _SkipChar);
			return HandelIpv6Digit(_Colons, _Numbers);
		}
		else
			return h_invalid;
	}
	return h_invalid;
}

HostState UriParser::PortHndler(size_t index)
{
	if (_Input[index] == '\0')
		return PushBuffer(_Uri->_Port, h_done);
	else if (std::isdigit(_Input[index]))
		return h_port;
	else
		return h_invalid;
}

HostState UriParser::RegNamePctDoneHndler(size_t index)
{
	CheckString(std::toupper, _Buf, _Buf.size() - 2);
	_Buf = DecodePercent(_Buf, _Buf.size() - 3);
	switch (_Input[index])
	{
	case '\0':
		return PushBuffer(_Uri->_Host, h_done);
	case '%':
		return h_regnamepct;
	default:
		if (IsUnreservedSubDelim(_Input[index]))
			return h_regname;
		else
			return h_invalid;
	}
}

HostState UriParser::RegNamePctHndler(size_t index)
{
	if (_Buf.back() == '%' && IsHexDig(_Input[index]))
		return h_regnamepct;
	else if (IsHexDig(_Buf.back()) && IsHexDig(_Input[index]))
		return h_regnamepctdone;
	return h_invalid;
}

HostState UriParser::Ipv4Hndler(size_t index)
{
	switch (_Input[index])
	{
	case '\0':
		if (_Groops == 4 && ValidDecOctetGroop(_Buf))
			return PushBuffer(_Uri->_Host, h_done);
		break;
	case ':':
		if (_Groops == 4)
			return PushBuffer(_Uri->_Host, h_port);
		break;
	case ']':
		if (_Lit && _Groops == 4)
			return h_literalend;
		break;
	case '.':
		if (std::isdigit(_Buf.back()) && ValidDecOctetGroop(_Buf))
			return h_Ipv4;
		break;
	default:
		if (std::isdigit(_Input[index]))
			return Handelipv4Digits(_Numbers, _Groops, _Buf);
		else
			return h_invalid;
	}
	return h_invalid;
}

// handels transition into IP literal, reg name or ipv4 parsing
HostState UriParser::StartHandler(size_t index)
{
	switch (_Input[index])
	{
	case '\0':
		return PushBuffer(_Uri->_Host, h_done);
	case '[':
		_Lit = true;
		return h_literal;
	case '%':
		return h_regnamepct;
	default:
		if (IsIpv4Format(_Input)) // check if its ipv4 format
			return h_Ipv4;
		else if (IsUnreservedSubDelim(_Input[index])) // otherwise we assume its a reg-name format
			return h_regname;
		else
			return h_invalid;
	}
}

HostState UriParser::GetNextState(size_t index)
{
	static HostState (UriParser::*tab[])(size_t i) = {
		&UriParser::StartHandler,
		&UriParser::LiteralHandler,
		&UriParser::Ipv6Hndler,
		&UriParser::IpvFHndler,
		&UriParser::Ipv4Hndler,
		&UriParser::LiteralEndHndler,
		&UriParser::RegNameHndler,
		&UriParser::RegNamePctHndler,
		&UriParser::RegNamePctDoneHndler,
		&UriParser::PortHndler,
		NULL};
	_SkipChar = false;
	return (this->*tab[_CurrentState])(index);
}

void UriParser::CheckInvalidState() const
{
	if (_CurrentState == h_invalid)
		throw std::invalid_argument("Error: invalid token in HOST Uri");
}

void UriParser::AfterParserCheck()
{
	if (_CurrentState == h_done && _Index < _Input.size() - 1)
		throw std::invalid_argument("Error: Charecters After Terminating Token In Host Uri");
}

void NormalizeIPv6HexDig(std::string &buff, char c, bool &skip_c)
{
	if (c >= 'A' && c <= 'Z')
	{
		buff += std::tolower(c);
		skip_c = true;
	}
}

HostState HandelIpv6Colone(size_t &colons, size_t &numbers, size_t &groops)
{
	if (colons > 1) // only a maximum of 2 colons in allowed
		return h_invalid;
	colons++;
	numbers = 0;
	groops++;
	return h_Ipv6;
}

HostState HandelIpv6Digit(size_t &colons, size_t &digit)
{
	if (digit > 3)
		return h_invalid;
	colons = 0;
	digit++;
	return h_Ipv6;
}

bool ValidLastBitsIpv4(std::string const &str, size_t pos)
{
	if (str[pos - 1] == ':' && std::isdigit(str[pos]))
	{
		size_t end = str.find_first_not_of(']', pos);
		std::string last_bits = str.substr(pos, end - pos);
		return (IsIpv4Format(last_bits));
	}
	return false;
}

HostState Handelipv4Digits(size_t &nums, size_t &groops, std::string const &buf)
{
	if (nums > 3)
		return h_invalid;
	char c = buf.back();
	if (c == '.' || groops == 0)
	{
		nums = 1;
		groops++;
	}
	else
		nums++;
	return h_Ipv4;
}
