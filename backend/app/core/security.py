"""Authentication and authorization helpers.

Research-only posture: bearer JWT placeholder. Replace with your provider.
"""

from fastapi import Depends, HTTPException, status
from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials
from jose import JWTError, jwt

from app.core.config import settings

bearer_scheme = HTTPBearer(auto_error=False)


async def get_current_user(
    credentials: HTTPAuthorizationCredentials | None = Depends(bearer_scheme),
):
    """Validate bearer token. Placeholder — replace with your auth provider.

    In development, returns a dummy authenticated user when no token is provided.
    In production, enforce token validation.
    """
    if settings.DEBUG and credentials is None:
        return {"sub": "dev-user", "role": "researcher"}

    if credentials is None:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Not authenticated",
            headers={"WWW-Authenticate": "Bearer"},
        )

    try:
        payload = jwt.decode(
            credentials.credentials,
            settings.SECRET_KEY,
            algorithms=[settings.ALGORITHM],
        )
        return payload
    except JWTError:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Invalid authentication credentials",
            headers={"WWW-Authenticate": "Bearer"},
        )


def require_researcher(user: dict = Depends(get_current_user)):
    """Ensure the caller has researcher-level access."""
    if user.get("role") not in ("researcher", "admin", "dev-user"):
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN,
            detail="Insufficient permissions",
        )
    return user
