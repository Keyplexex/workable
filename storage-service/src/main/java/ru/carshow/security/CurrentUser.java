package ru.carshow.security;

import org.springframework.security.core.Authentication;
import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.oauth2.jwt.Jwt;
import org.springframework.stereotype.Component;

import java.util.UUID;
import java.util.stream.Collectors;

@Component
public class CurrentUser {
    
    public UUID getId() {
        Authentication authentication = SecurityContextHolder.getContext().getAuthentication();
        if (authentication != null && authentication.getPrincipal() instanceof Jwt jwt) {
            String sub = jwt.getSubject(); 
            if (sub != null) {
                return UUID.fromString(sub);
            }
        }
        throw new IllegalStateException("User not authenticated");
    }
    
    public boolean hasRole(String role) {
        Authentication authentication = SecurityContextHolder.getContext().getAuthentication();
        
        if (authentication == null) {
            System.err.println("[DEBUG] No authentication found!");
            return false;
        }
        
        String authorities = authentication.getAuthorities().stream()
            .map(GrantedAuthority::getAuthority)
            .collect(Collectors.joining(", "));
        
        System.out.println("[DEBUG] User: " + authentication.getName());
        System.out.println("[DEBUG] Authorities: [" + authorities + "]");
        
        boolean hasRoleWithPrefix = authentication.getAuthorities().stream()
            .anyMatch(a -> a.getAuthority().equals("ROLE_" + role));
        
        boolean hasRoleWithoutPrefix = authentication.getAuthorities().stream()
            .anyMatch(a -> a.getAuthority().equals(role));
        
        System.out.println("[DEBUG] Has role '" + role + "': " + 
            (hasRoleWithPrefix || hasRoleWithoutPrefix) + 
            " (with prefix: " + hasRoleWithPrefix + ", without: " + hasRoleWithoutPrefix + ")");
        
        return hasRoleWithPrefix || hasRoleWithoutPrefix;
    }
    
    public String getUsername() {
        Authentication authentication = SecurityContextHolder.getContext().getAuthentication();
        if (authentication != null && authentication.getPrincipal() instanceof Jwt jwt) {
            return jwt.getClaimAsString("preferred_username");
        }
        return "anonymous";
    }
}