package ru.carshow.config;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.core.convert.converter.Converter;
import org.springframework.security.authentication.AbstractAuthenticationToken;
import org.springframework.security.config.annotation.method.configuration.EnableMethodSecurity;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.config.http.SessionCreationPolicy;
import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.authority.SimpleGrantedAuthority;
import org.springframework.security.oauth2.jwt.Jwt;
import org.springframework.security.oauth2.server.resource.authentication.JwtAuthenticationConverter;
import org.springframework.security.oauth2.server.resource.authentication.JwtAuthenticationToken;
import org.springframework.security.web.SecurityFilterChain;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.Map;

@Configuration
@EnableWebSecurity
@EnableMethodSecurity(prePostEnabled = true)
public class SecurityConfig {

    @Bean
    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
        http
            .csrf(csrf -> csrf.disable())
            .sessionManagement(session -> 
                session.sessionCreationPolicy(SessionCreationPolicy.STATELESS))
            .authorizeHttpRequests(auth -> auth
                .requestMatchers(
                    "/swagger-ui/**",
                    "/swagger-ui.html",
                    "/v3/api-docs/**",
                    "/v3/api-docs",
                    "/actuator/health",
                    "/api/internal/**"
                ).permitAll()
                .anyRequest().authenticated()
            )
            .oauth2ResourceServer(oauth2 -> 
                oauth2.jwt(jwt -> 
                    jwt.jwtAuthenticationConverter(jwtAuthenticationConverter())
                )
            );
        
        return http.build();
    }

    @Bean
    public JwtAuthenticationConverter jwtAuthenticationConverter() {
        JwtAuthenticationConverter converter = new JwtAuthenticationConverter();
        converter.setJwtGrantedAuthoritiesConverter(new KeycloakRoleConverter());
        return converter;
    }

    static class KeycloakRoleConverter implements Converter<Jwt, Collection<GrantedAuthority>> {
        @Override
        public Collection<GrantedAuthority> convert(Jwt jwt) {
            System.out.println("[CONVERTER] Full token claims: " + jwt.getClaims().keySet());
            
            Map<String, Object> realmAccess = jwt.getClaim("realm_access");
            System.out.println("[CONVERTER] realm_access: " + realmAccess);
            
            if (realmAccess == null) {
                System.err.println("[CONVERTER] realm_access is NULL in token!");
                return Collections.emptyList();
            }
            
            Object rolesObj = realmAccess.get("roles");
            System.out.println("[CONVERTER] Raw roles: " + rolesObj);
            
            if (rolesObj instanceof List<?> rolesList) {
                List<GrantedAuthority> authorities = new ArrayList<>();
                for (Object role : rolesList) {
                    if (role instanceof String roleName) {
                        String authorityName = "ROLE_" + roleName;
                        authorities.add(new SimpleGrantedAuthority(authorityName));
                        System.out.println("[CONVERTER] Added authority: " + authorityName);
                    }
                }
                System.out.println("[CONVERTER] Total authorities: " + authorities.size());
                return authorities;
            }
            
            System.err.println("[CONVERTER] roles is not a List: " + (rolesObj != null ? rolesObj.getClass() : "null"));
            return Collections.emptyList();
        }
    }
}